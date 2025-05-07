cmake_minimum_required(VERSION 3.6)
project(proxyserver)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    add_compile_options(-Wall -ggdb -pipe -std=c++11)

    add_definitions(-D__LINUX__ -DSPDLOG_NO_NAME -DCOMPILE_LUA_WITH_CXX)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-D__WINDOWS__ -D_DEBUG_ -DWIN32_LEAN_AND_MEAN -DSPDLOG_NO_NAME -DCOMPILE_LUA_WITH_CXX)
endif()

# 仅对 Visual Studio 编译器生效
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

set(SOURCE_FILES
    "main.cpp" 
    "../common/message/message.pb.cc" 
    "../common/message/message.pb.h"
    "../common/tcp_def.h"
    "../common/server_config.h"
    "../common/server_config.cpp"
    "inc/proxy_ctrl.h" 
    "inc/proxy_def.h"
    "inc/proxy_server.h" 
    "inc/proxy_player.h"
    "inc/proxy_message.h"
    "inc/mfactory_manager.h"
    "inc/proxy_transfer.h"
    "src/proxy_ctrl.cpp" 
    "src/proxy_server.cpp"
    "src/proxy_player.cpp"
    "src/mfactory_manager.cpp"
    "src/proxy_transfer.cpp")

include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/common/message
        ${CMAKE_SOURCE_DIR}/lua-5.3.6/
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/shm
        ${CMAKE_SOURCE_DIR}/framework/message
        ${CMAKE_SOURCE_DIR}/framework/net
        ${CMAKE_SOURCE_DIR}/framework/thread
        ${CMAKE_SOURCE_DIR}/framework/std
        ${CMAKE_SOURCE_DIR}/framework/lua
        ${CMAKE_SOURCE_DIR}/framework/db
        ${CMAKE_SOURCE_DIR}/proxyserver/inc
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
        ${CMAKE_SOURCE_DIR}/lib/Debug
endif()
)

add_executable(proxyserver ${SOURCE_FILES})
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(proxyserver framework)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(proxyserver framework)
endif()

