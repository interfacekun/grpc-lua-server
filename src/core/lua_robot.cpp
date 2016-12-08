
#include "../net/UdpGzip.h"
#include <iostream>
#include "lua_robot.h"
#include "calldata.h"
#include "scheduler.h"

inline int ErrorHandler(lua_State *L) {
    // stack trace
    // call debug.traceback
    lua_getglobal(L, "debug");
    lua_getfield(L, -1, "traceback");
    lua_pushvalue(L, 1);
    lua_pushinteger(L, 2);
    lua_call(L, 2, 1);

    // error code
    std::string errMsg = "ErrorCode:\n";
    if (!lua_isnil(L, -1)) {
        errMsg.append(lua_tostring(L, -1 ));
    }
    LogError("%s",errMsg.c_str());
    return 1;
}

inline int SetErrorHandler(lua_State *L) {
    lua_pushcfunction(L, &ErrorHandler);
    return lua_gettop(L);
}

LuaRobot::~LuaRobot(){
   if(!L){
       lua_close(L);
   }
}

int LuaRobot::init(int index){
    L = luaL_newstate();
    if (!L) {
        LogError("luaL_newstate error");
        return -1;
    }

    luaL_openlibs(L);
    luaopen_ak(L);

    _errorHandler = SetErrorHandler(L);
    if (luaL_loadfile(L,"./lua/main.lua") != 0 ){
            std::string errorMsg = "Loading script failed:";
            errorMsg.append(lua_tostring(L, -1));
            LogError("%s",errorMsg.c_str());
            return -1;
    }
    if (lua_pcall(L, 0, 0, _errorHandler) != 0) {
        LogError("%s","Run first script error");
        return -1;
    }
    _robot_index=index;
    return 0;
}

void LuaRobot::update(){

    if (_data == NULL) {
        return;
    }

    CallData *cdata=static_cast<CallData*>(_data);

    if (_blInit !=true) {

        _lastFrameNo=0;
        _currentFrameNo=0;
        _t_running_time_update=0;

        _t_start = std::chrono::steady_clock::now();
        _time_last_update = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _t_start).count();

        lua_gc(L, LUA_GCCOLLECT, 0);

        if (!luaInit()) {
            _blInit=false;
            _data=NULL;
            cdata->setstatus(CallData::CallStatus::WRITE);
            cdata->Proceed();
            return;
        }
        lua_gc(L, LUA_GCCOUNT, 0);
        _blInit=true;
         LogInfo("robot start!battleid:%d robot_index:%d",int(cdata->getmsg().clanid()),_robot_index);
    }

    if (_udpclient !=NULL) {
        _udpclient->nonBlockingEventLoop();
        pollMsg();
    }

    if (luaUpdate(0)==false) {

        _blInit=false;
        _data=NULL;
        cdata->setstatus(CallData::CallStatus::WRITE);
        cdata->Proceed();

        _udpclient->close();
        _udpclient=NULL;

        auto time_cost=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _t_start).count();
        LogInfo("robot stop!index:%d protoid:%d battle-id:%d done!cost:%d ms",_robot_index,cdata->getmsg().protoid(),int(cdata->getmsg().clanid()),int(time_cost));
        lua_gc(L, LUA_GCCOLLECT, 0);
    }
}

bool  LuaRobot::luaInit() {
    CallData *cdata=static_cast<CallData*>(_data);
    lua_getglobal(L, "init");
    if (!lua_isfunction(L, -1)){
        lua_pop(L, 1);
    }

    lua_pushnumber(L, _robot_index);
    lua_pushnumber(L, cdata->getmsg().protoid());
    lua_pushlstring (L, (const char *)cdata->getmsg().payload().c_str(),cdata->getmsg().payload().size());
    if (lua_pcall(L, 3, 0, _errorHandler) != 0) {
        std::string errorMsg = "call ./sg/serverrobot/main.lua:init error! ";
        errorMsg.append(lua_tostring(L, -1));
        LogError("%s",errorMsg.c_str());
        return false;
    }
    return true;
}

bool LuaRobot::luaUdpRpcSuccess(int type, const std::string &responseBytes) {
        lua_getglobal(L, "onUdpRpcSuccess");
        if (!lua_isfunction(L, -1)){
            lua_pop(L, 1);
        }
        lua_pushnumber(L, type);
        lua_pushlstring(L, responseBytes.c_str(), responseBytes.size());

        if (lua_pcall(L, 2, 0, _errorHandler) != 0) {
            LogError("onRpcSueccess error");
            return false;
        }
        return true;
}

bool LuaRobot::luaUdpRpcError(int code, const std::string &message) {
        lua_getglobal(L, "onUdpRpcError");
        if (!lua_isfunction(L, -1))
        {
            lua_pop(L, 1);
        }
        lua_pushnumber(L, code);
        lua_pushstring(L, message.c_str());
        if (lua_pcall(L, 2, 0, _errorHandler) != 0) {
            LogError("onRpcError error");
            return false;
        }
        return true;
}

bool LuaRobot::luaUpdate(float nFrames) {

    CallData *cdata=static_cast<CallData*>(_data);
    if (cdata == NULL) {
        return false;
    }

    lua_getglobal(L, "update");
    if (!lua_isfunction(L, -1)){
        lua_pop(L, 2);
        return false;
    }
    lua_pushnumber(L, nFrames);
    if (lua_pcall(L, 1, 1, _errorHandler) != 0) {
        LogError("Update error!battleid:%d",int(cdata->getmsg().clanid()));
        return false;
    }
    if (!lua_isnumber(L, -1)) {
        LogError("Update error return is not number!battleid:%d",int(cdata->getmsg().clanid()));
        return false;
    }
    int ret= (int)lua_tonumber(L, -1);
    lua_pop(L, 1);
    return ret==0;
}

void LuaRobot::pollMsg(){
    if (_udpclient->canPollMsg()) {
        _udpclient->poll([this](const ak::UdpMessage *msg, const boost::system::error_code *ec) {
            if (ec != nullptr) {
                this->luaUdpRpcError(ec->value(), ec->message());
                return;
            } else {
                this->luaUdpRpcSuccess(msg->ProtoID(), std::string(msg->body(), msg->body_length()));
            }
        });
    }
}

void LuaRobot::setRequest(void* data){
    if (data==NULL || _data!=NULL ) {
        return ;
    }
    _data=data;
    return;
}

void LuaRobot::connectToUdpServer(int robot_index,std::string ip ,std::string port ){
    LuaRobot* luarobot=Scheduler::getInstance()->getLuaRobotByIndex(robot_index);
    assert(luarobot != nullptr);
    if (luarobot->_udpclient != NULL) {
        luarobot->_udpclient->close();
        luarobot->_udpclient=NULL;
    }
    LogInfo("Create new connection to udp server %s : %s\n", ip.c_str(), port.c_str());
    luarobot->_udpclient = std::unique_ptr<ak::UdpClient>(new ak::UdpClient(ip, port,true));
}

void LuaRobot::callUdpRpc(int robot_index ,int protoId, std::string buffer) {
    LuaRobot* luarobot=Scheduler::getInstance()->getLuaRobotByIndex(robot_index);
    assert(luarobot != nullptr);
    auto m = new ak::UdpMessage(protoId, buffer.length());
    memcpy(m->body(), buffer.c_str(), buffer.length());
    luarobot->_udpclient->write(m);
}

long LuaRobot::getDeltaTimeSince(const long anchorTime){
    return  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() -anchorTime;
}
