cmake_minimum_required(VERSION 3.6)
project(proxyserver)
add_definitions(-D_DEBUG_ -DUSE_LOG4CPP -DMEDIUM_SIZE -D_POSIX_MT_ -DLINUX -DUSE_MY_ASSERT)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -pipe -std=c++11")

set(SOURCE_FILES
        main.cpp
        inc/proxy_conn.h
        inc/proxy_ctrl.h
        inc/proxy_handle.h
        src/proxy_conn.cpp
        src/proxy_ctrl.cpp
        src/proxy_handle.cpp
        )

include_directories(
        /usr/local/include/
        ${CMAKE_SOURCE_DIR/framework/include}
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

link_directories(
        /usr/local/lib/
        ${CMAKE_SOURCE_DIR}/lib
)

add_executable(proxyserver ${SOURCE_FILES})
target_link_libraries(proxyserver framework pthread protobuf crypto)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)