cmake_minimum_required(VERSION 3.6)
project(myserver)
add_definitions(-D_DEBUG_ -DUSE_LOG4CPP -DUSE_MY_ASSERT -D_POSIX_MT_)

set(FRAMEWORK
        base/base.h
        base/object.h
        base/performance.h
        base/servertool.h
        base/base.cpp
        base/performance.cpp
        base/servertool.cpp
        base/my_assert.h
        base/my_macro.h
        base/my_macro.h
        base/objectManager.cpp
        base/objectManager.h
        const/commondef.h
        const/errcode.h
        const/dblogintype.h
        json/json.h
        json/json.cpp
        json/config.h
        json/config.cpp
        log/log.h
        log/log.cpp
        message/message.pb.h
        message/message.pb.cc
        message/player.pb.h
        message/player.pb.cc
        message/dbmessage.pb.h
        message/dbmessage.pb.cc
        message/common.pb.h
        message/common.pb.cc
        net/statistics.h
        net/tcp_conn.h
        net/runflag.h
        net/daemon.h
        net/oi_tea.h
        net/daemon.cpp
        net/oi_tea.cpp
        net/runflag.cpp
        net/statistics.cpp
        net/ccrypto.h
        net/ccrypto.cpp
        net/client_comm_engine.h
        net/server_comm_engine.h
        net/message_interface.h
        net/message_interface.cpp
        net/server_comm_engine.cpp
        net/client_comm_engine.cpp
        mem/sharemem.h
        mem/shm.h/
        mem/sharemem.cpp
        mem/shm.cpp
        mem/codequeue.h
        mem/codequeue.cpp
        mem/mempool.h
        mem/mempoolsafty.h
        thread/mythread.h
        thread/mythread.cpp
        timer/clock.cpp
        timer/clock.h
        timer/timer.h
        timer/timer.cpp
        event/event.h
        event/eventbus.h
        event/eventHandler.h
        event/eventbus.cpp
        event/handlerregistration.h)

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
