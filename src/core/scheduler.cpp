#include "scheduler.h"
#include "../net/platform.h"
#include "calldata.h"

Scheduler* Scheduler::_scheduler = NULL;
Scheduler * Scheduler::getInstance(){
    if( _scheduler==NULL){
        _scheduler = new Scheduler();
    }
    return _scheduler;
}

Scheduler::Scheduler(){
    _robot_num =0;
}

void Scheduler::init(int robot_num){
    _robot_num=robot_num;
    for (int i = 0; i < _robot_num; i++) {
        LuaRobot *lr=new LuaRobot();
        lr->init(i);
        _vec_luarobot.push_back(lr);
    }
    boost::thread* thread = new boost::thread(boost::bind(&Scheduler::run, this));
    _threads.push_back(thread);
    LogInfo("Scheduler init ok! %d",_robot_num);
}

void Scheduler::run(){
    while(true){
        for(std::vector<LuaRobot*>::iterator it  = _vec_luarobot.begin(); it != _vec_luarobot.end(); ++it){
            (*it)->update();
        }
    }
}

void Scheduler::dispatch(void* data){
    if (data==NULL) {
        return ;
    }
    LogDebug("enter Scheduler dispatch!");

    //if find a available thread
    for(std::vector<LuaRobot*>::iterator it  = _vec_luarobot.begin(); it != _vec_luarobot.end(); ++it){
        if ((*it)->isProcessing() == false) {
            (*it)->setRequest(data);
            return;
        }
    }

    //if not found
    LogError("dispatch error,can not find a robot now!");
    CallData *cdata=static_cast<CallData*>(data);
    cdata->setstatus(CallData::CallStatus::WRITE);
    cdata->Proceed();
}

LuaRobot* Scheduler::getLuaRobotByThread(){
    auto id =boost::this_thread::get_id();
    for (int i = 0; i < _robot_num; i++) {
        if(_threads[i]->get_id() == id)
        {
            return _vec_luarobot[i];
        }
    }
    return NULL;
}

LuaRobot* Scheduler::getLuaRobotByIndex(int index){
    if (index < 0 || index >=_robot_num) {
        return NULL;
    }
    return _vec_luarobot[index];
}

void Scheduler::jionAllThreads(){
    for(std::vector<boost::thread*>::iterator it  = _threads.begin(); it != _threads.end(); ++it){
        (*it)->join();
    }
}
