cmake_minimum_required(VERSION 3.6)
project(tcpserver)
add_definitions(-D_DEBUG_ -DUSE_LOG4CPP -DMEDIUM_SIZE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -std=c++11")

set(SOURCE_FILES
        main.cpp
        inc/commdef.h
        inc/tcpctrl.h
        src/tcpctrl.cpp
        )

link_directories(
        /usr/local/lib/
)

add_executable(tcpserver ${SOURCE_FILES})
target_link_libraries(tcpserver framework pthread protobuf crypto)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)