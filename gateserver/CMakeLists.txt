cmake_minimum_required(VERSION 3.6)
project(gateserver)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    add_compile_options(-Wall -ggdb -pipe -std=c++14)

    add_definitions(-D__LINUX__ -D_DEBUG_ -DSPDLOG_NO_NAME)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-D__WINDOWS__ -D_DEBUG_ -DSPDLOG_NO_NAME)
endif()

# 仅对 Visual Studio 编译器生效
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

set(SOURCE_FILES
    "main.cpp" 
    "inc/gate_ctrl.h" 
    "inc/gate_def.h"
    
    "inc/gate_server.h" 
    "src/gate_ctrl.cpp" 
    
    "src/gate_server.cpp"
    "../common/message/message.pb.cc" 
    "../common/message/message.pb.h"
    "inc/game_player.h"
    "src/game_player.cpp"
    "../common/common_def.h")

include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/common/message
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/const
        ${CMAKE_SOURCE_DIR}/framework/json
        ${CMAKE_SOURCE_DIR}/framework/log
        ${CMAKE_SOURCE_DIR}/framework/mem
        ${CMAKE_SOURCE_DIR}/framework/net
        ${CMAKE_SOURCE_DIR}/framework/thread
        ${CMAKE_SOURCE_DIR}/framework/timer
        ${CMAKE_SOURCE_DIR}/gateserver/inc
)

link_directories(
        ${CMAKE_SOURCE_DIR}/lib
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        /usr/lib
        /usr/local/lib
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/lib/linux
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        C:/Windows/System32
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/lib/win64
endif()
)

add_executable(gateserver ${SOURCE_FILES})
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(gateserver framework pthread)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(gateserver framework protobufd)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run)