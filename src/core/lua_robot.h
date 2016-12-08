#ifndef LUA_ROBOT
#define LUA_ROBOT
extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
    int     luaopen_ak(lua_State *L);
}  // extern "C"

#include <queue>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include "../net/UdpClient.h"


class LuaRobot :private boost::noncopyable {
protected:
        static LuaRobot *_instance;
public:
    int     init(int index);
    void    update();
    void    setRequest(void* data);
    bool    isProcessing(){return _data != NULL; };
public:
    static void    connectToUdpServer(int index,std::string ip ,std::string port);
    static void    callUdpRpc(int index,int protoId, std::string buffer);
    static long    getDeltaTimeSince(const long anchorTime);
private:
    void*   getData();
    ~LuaRobot();

    bool    luaInit();
    bool    luaUdpRpcSuccess(int type, const std::string &responseBytes);
    bool    luaUdpRpcError(int code, const std::string &message);
    bool    luaFinished();
    bool    luaUpdate(float nFrames);
    void    pollMsg();
    bool    framesUpdate();

private:
    int _robot_index=-1;
    lua_State   *L;
    int _errorHandler = 0;
    bool _blInit=false;
    void * _data;
    std::unique_ptr<ak::UdpClient> _udpclient;
    boost::thread* _update_loop_thread;
    unsigned char _uncompress_buff[40960];

    std::chrono::steady_clock::time_point _t_start;
    long _time_last_update;
    int _currentFrameNo;
    int _lastFrameNo;
    long _t_running_time_update;
};

#endif
