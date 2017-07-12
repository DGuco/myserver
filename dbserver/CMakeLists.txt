cmake_minimum_required(VERSION 3.6)
project(dbeserver)
add_definitions(-D_DEBUG_ -DUSE_LOG4CPP -DUSE_MY_ASSERT)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -pipe -std=c++11")

set(SOURCE_FILES
        main.cpp)

link_directories(
        /usr/local/lib/
        ${CMAKE_SOURCE_DIR/framework/base}
        ${CMAKE_SOURCE_DIR/framework/config}
        ${CMAKE_SOURCE_DIR/framework/log}
        ${CMAKE_SOURCE_DIR/framework/message}
        ${CMAKE_SOURCE_DIR/framework/mystd}
        ${CMAKE_SOURCE_DIR/framework/net}
        ${CMAKE_SOURCE_DIR/framework/sharemem}
        ${CMAKE_SOURCE_DIR/framework/thread}
        ${CMAKE_SOURCE_DIR/framework/timer}
)

add_executable(dbeserver ${SOURCE_FILES})
target_link_libraries(dbeserver framework protobuf crypto)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)