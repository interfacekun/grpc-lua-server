/*
    This is the swig interface file for ak
    Please run the following command to generate the wrap classes for lua
    if using brew
    swig -c++ -lua -I../../../../royal/Engine/include -o ak_wrap.cpp ak.i

    if using mac port:
    swig -c++ -lua -I../../../include -I/opt/local/share/swig/3.0.2/ -o ak_wrap.cpp ak.i
*/

%module ak
%{
#include "../core/lua_robot.h"
using namespace ak;
%}


%wrapper %{
    extern lua_State *_l;
%}

// math
%include <std_string.i>
%ignore LuaRobot::init();
%ignore LuaRobot::run();
%ignore LuaRobot::pushRequest(void* data);
%include "../core/lua_robot.h"


%init %{
%}
