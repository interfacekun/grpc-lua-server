#ifndef SCHEDULER
#define SCHEDULER
#include <boost/thread/thread.hpp>
#include "lua_robot.h"

class Scheduler final  {
public:
  static Scheduler * getInstance();
  LuaRobot* getLuaRobotByThread();
  LuaRobot* getLuaRobotByIndex(int index);
  void init(int robot_num);
  void dispatch(void* data);
  void jionAllThreads();
private:
    Scheduler();
    ~Scheduler() {};
    void run();
private:
     static Scheduler* _scheduler;
     int _robot_num;
     std::vector<boost::thread*> _threads;
     std::vector<LuaRobot*> _vec_luarobot;
};

#endif
