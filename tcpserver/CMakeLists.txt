cmake_minimum_required(VERSION 3.6)
project(tcpserver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        main.cpp
        inc/commdef.h
        inc/tcpctrl.h
        src/tcpctrl.cpp
        )

include_directories(
        /usr/local/include
        ${CMAKE_SOURCE_DIR/common/message/inc}
        ${CMAKE_SOURCE_DIR/common/mytimer}
        ${CMAKE_SOURCE_DIR/common/tools/inc}
)
link_directories(
        /usr/local/lib/
        ${CMAKE_SOURCE_DIR}/lib
)

add_executable(tcpserver ${SOURCE_FILES})
target_link_libraries(tcpserver common pthread protobuf )
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)