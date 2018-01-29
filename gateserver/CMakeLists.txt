cmake_minimum_required(VERSION 3.6)
project(gateserver)
add_definitions(-D_DEBUG_ -DUSE_LOG4CPP -DMEDIUM_SIZE -D_TGW_ -DUSE_MY_ASSERT -DLINUX)
#-g -D_DEBUG_ -DUSE_LOG4CPP -DSMALL_SIZE

set(SOURCE_FILES
        main.cpp
        inc/gatedef.h
        inc/gatectrl.h
        src/gatectrl.cpp)

include_directories(
        /usr/include
        /usr/local/include/
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
        /usr/local/lib/
        ${CMAKE_SOURCE_DIR}/lib
)

add_executable(gateserver ${SOURCE_FILES})
target_link_libraries(gateserver framework pthread protobuf crypto)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)