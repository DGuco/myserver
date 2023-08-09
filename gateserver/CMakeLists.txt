cmake_minimum_required(VERSION 3.6)
project(gateserver)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    add_compile_options(-Wall -ggdb -pipe -std=c++14)

    #libevent 多线程开启-DEVENT_THREAD_SAFE
    add_definitions(-D__LINUX__ -D_DEBUG_ -DUSE_MY_ASSERT)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    #libevent 多线程开启-DEVENT_THREAD_SAFE
    add_definitions(-D__WINDOWS__ -D_DEBUG_ -DUSE_MY_ASSERT -D_USING_V110_SDK71_)
endif()

set(SOURCE_FILES
    "main.cpp" 
    "inc/gate_ctrl.h" 
    "inc/gate_def.h"
    "inc/mes_handle.h"
    "inc/net_manager.h" 
    "src/gate_ctrl.cpp" 
    "src/mes_handle.cpp"
    "src/net_manager.cpp"
    "../common/message/common.pb.h" 
    "../common/message/message.pb.h")

include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/const
        ${CMAKE_SOURCE_DIR}/framework/json
        ${CMAKE_SOURCE_DIR}/framework/log
        ${CMAKE_SOURCE_DIR}/framework/mem
        ${CMAKE_SOURCE_DIR}/framework/net
        ${CMAKE_SOURCE_DIR}/framework/thread
        ${CMAKE_SOURCE_DIR}/framework/timer
)

link_directories(
        ${CMAKE_SOURCE_DIR}/lib
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        /usr/lib
        /usr/local/lib
        ${CMAKE_SOURCE_DIR}/lib/linux
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        C:/Windows/System32
        ${CMAKE_SOURCE_DIR}/lib/win64
endif()
)

add_executable(gateserver ${SOURCE_FILES})
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(gateserver framework pthread protobuf crypto)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(gateserver framework ws2_32 protobuf crypto)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)