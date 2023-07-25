cmake_minimum_required(VERSION 3.6)
project(proxyserver)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions(-D_DEBUG_ -DUSE_MY_ASSERT -DEVENT_THREAD_SAFE)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    add_compile_options(-Wall -ggdb -pipe -std=c++14)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")

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
        ${CMAKE_SOURCE_DIR}/lib/linux
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        ${CMAKE_SOURCE_DIR}/lib/win64
endif()
)

add_executable(proxyserver ${HEAD_FILES} ${SOURCE_FILES})
target_link_libraries(proxyserver framework pthread protobuf crypto)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)