cmake_minimum_required(VERSION 2.8)
project(robot)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/src/cmake/Modules/")
include(BuildHelpers)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions("-DDEBUG")
endif()

if(LINUX)
    if(EXISTS "/etc/debian_version")
       set ( LINUX_VERSION "Debian")
    elseif(EXISTS "/etc/redhat-release")
       set ( LINUX_VERSION "Redhat")
    endif()
endif()

#for log
add_definitions("-DSERVER")

if(MACOSX)
    SET (CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS}  -std=c++11")
    SET (CMAKE_CXX_FLAGS_DEBUG          "-g -O0  ")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-O0 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O0 -g")
elseif(LINUX)
    add_definitions("-DLINUX")
    SET (CMAKE_CXX_COMPILER             "/usr/bin/g++")
    SET (CMAKE_CXX_FLAGS                " -Wall -std=c++11")
    SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
endif()

include_directories(${PROJECT_SOURCE_DIR}/thirdpart/include/protobuf/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdpart/include/luajit2.0.3/Include)
include_directories(${PROJECT_SOURCE_DIR}/thirdpart/include)

if (MACOSX)
    include_directories(${PROJECT_SOURCE_DIR}/thirdpart/include/grpc/OSX)
    LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdpart/lib/OSX/boost/)
    LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdpart/lib/OSX/protobuf/)
    LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdpart/lib/OSX/grpc/)
    LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdpart/lib/OSX/luajit/)
    SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pagezero_size 10000 -image_base 100000000" )
    SET( LIB_LUAJIT  libluajit.a)
elseif (LINUX)
    include_directories(${PROJECT_SOURCE_DIR}/thirdpart/include/grpc/LINUX)
    LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdpart/lib/${LINUX_VERSION}/boost/)
    LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdpart/lib/${LINUX_VERSION}/protobuf/)
    LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdpart/lib/${LINUX_VERSION}/grpc/)
    LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdpart/lib/${LINUX_VERSION}/luajit/)
    SET( LIB_LUAJIT  libluajit-5.1.a)
endif()


file(GLOB AK_SRC
  ${PROJECT_SOURCE_DIR}/src/lua/*.cpp
)

add_library(AK${CMAKE_BUILD_TYPE} STATIC ${AK_SRC})

file( GLOB ROBOT_SRC
    ${PROJECT_SOURCE_DIR}/src/net/UdpGzip.cpp
    ${PROJECT_SOURCE_DIR}/src/net/UdpClient.cpp
    ${PROJECT_SOURCE_DIR}/src/core/scheduler.cpp
    ${PROJECT_SOURCE_DIR}/src/core/main.cpp
    ${PROJECT_SOURCE_DIR}/src/core/lua_robot.cpp
    ${PROJECT_SOURCE_DIR}/src/grpc/services.grpc.pb.cc
    ${PROJECT_SOURCE_DIR}/src/grpc/services.pb.cc
)

SET (LIBRARY_OUTPUT_PATH    "${PROJECT_SOURCE_DIR}/output/lib/${SYSTEM_STRING}")
SET (EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/output/bin/${SYSTEM_STRING}")

LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})
add_executable(robot ${ROBOT_SRC})
target_link_libraries(robot AK${CMAKE_BUILD_TYPE} z pthread dl ${LIB_LUAJIT} libboost_filesystem.a libboost_system.a libboost_thread.a libprotobuf.a libgpr.a libgrpc++.a libgrpc.a)

set_target_properties(robot PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/output/bin/${SYSTEM_STRING}
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/output/bin/${SYSTEM_STRING}
)
