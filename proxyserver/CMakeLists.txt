cmake_minimum_required(VERSION 3.6)
project(proxyserver)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    add_compile_options(-Wall -ggdb -pipe -std=c++14)

    #libevent 多线程开启-DEVENT_THREAD_SAFE
    add_definitions(-D_LINUX_ -D_DEBUG_ -DUSE_MY_ASSERT)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    #libevent 多线程开启-DEVENT_THREAD_SAFE
    add_definitions(-D_WINDOWS_ -DWIN32 -D_WIN32_ -D_WIN32 -D_X86_ -D_DEBUG_ -DUSE_MY_ASSERT)
endif()

set(HEAD_FILES
        inc/proxy_ctrl.h
        )

set(SOURCE_FILES
        src/proxy_ctrl.cpp
        main.cpp
        )
include_directories(
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/const
        ${CMAKE_SOURCE_DIR}/framework/json
        ${CMAKE_SOURCE_DIR}/framework/log
        ${CMAKE_SOURCE_DIR}/framework/mem
        ${CMAKE_SOURCE_DIR}/framework/message
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

add_executable(proxyserver ${HEAD_FILES} ${SOURCE_FILES})

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(proxyserver framework pthread protobuf crypto)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(proxyserver framework ws2_32 protobuf crypto)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)