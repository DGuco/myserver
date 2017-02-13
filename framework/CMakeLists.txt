cmake_minimum_required(VERSION 3.6)
add_definitions(-DUSE_LOG4CPP -DLINUX -DUSE_MY_ASSERT -std=c++11)
project(myserver)
set(FRAMEWORK
        base/base.h
        base/object.h
        base/objmanager.h
        base/dirtyword.h
        base/performance.h
        base/servertool.h
        base/base.cpp
        base/dirtyword.cpp
        base/object.cpp
        base/performance.cpp
        base/servertool.cpp
        base/my_assert.h
        json/json.h
        json/json.cpp
        log/log.h
        log/log.cpp
        message/message.pb.h
        message/tcpmessage.pb.h
        message/tcpmessage.pb.cc
        message/message.pb.cc
        message/proxymessage.pb.h
        message/proxymessage.pb.cc
        message/configure.pb.h
        message/configure.pb.cc
        net/statistics.h
        net/tcp_conn.h
        net/client_comm_engine.h
        net/runflag.h
        net/server_comm_engine.h
        net/daemon.h
        net/message_interface.h
        net/nethead.h
        net/oi_tea.h
        net/daemon.cpp
        net/message_interface.cpp
        net/nethead.cpp
        net/oi_tea.cpp
        net/runflag.cpp
        net/server_comm_engine.cpp
        net/statistics.cpp
        net/client_comm_engine.cpp
        net/ccrypto.h
        net/ccrypto.cpp
        sharemem/sharemem.h
        sharemem/shm.h/
        sharemem/sharemem.cpp
        sharemem/shm.cpp
        sharemem/code_queue.h
        sharemem/code_queue.cpp
        thread/thread.h
        thread/thread.cpp
        timer/clock.cpp
        timer/clock.h
        )

include_directories(
        /usr/local/include
)

link_directories(
        /usr/local/lib
        ${CMAKE_SOURCE_DIR}/lib
)

add_library(framework ${FRAMEWORK})
target_link_libraries(framework pthread protobuf log4cpp crypto)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
