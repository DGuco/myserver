cmake_minimum_required(VERSION 3.6)
add_definitions(-std=c++11)
project(gameserver)
set(COMMON
        tools/inc/array_list.h
        tools/inc/base.h
        tools/inc/client_comm_engine.h
        tools/inc/clock.h
        tools/inc/code_queue.h
        tools/inc/config.h
        tools/inc/daemon.h
        tools/inc/dirtyword.h
        tools/inc/hash_function.h
        tools/inc/log.h
        tools/inc/message_interface.h
        tools/inc/mymacro.h
        tools/inc/nethead.h
        tools/inc/object.h
        tools/inc/objmanager.h
        tools/inc/oi_tea.h
        tools/inc/performance.h
        tools/inc/protoconfig.h
        tools/inc/runflag.h
        tools/inc/server_comm_engine.h
        tools/inc/servertool.h
        tools/inc/sharemem.h
        tools/inc/shm.h/
        tools/inc/statistics.h
        tools/inc/tcp_conn.h
        tools/inc/thread.h
        tools/inc/json.h
        tools/inc/yq_assert.h
        tools/inc/config_data.h
        tools/src/base.cpp
        tools/src/client_comm_engine.cpp
        tools/src/clock.cpp
        tools/src/code_queue.cpp
        tools/src/config.cpp
        tools/src/daemon.cpp
        tools/src/dirtyword.cpp
        tools/src/log.cpp
        tools/src/message_interface.cpp
        tools/src/nethead.cpp
        tools/src/object.cpp
        tools/src/oi_tea.cpp
        tools/src/protoconfig.cpp
        tools/src/performance.cpp
        tools/src/runflag.cpp
        tools/src/server_comm_engine.cpp
        tools/src/servertool.cpp
        tools/src/sharemem.cpp
        tools/src/shm.cpp
        tools/src/statistics.cpp
        tools/src/thread.cpp
        tools/src/json.cpp
        tools/src/config_data.cpp
        message/message.pb.h
        message/tcpmessage.pb.h
        message/tcpmessage.pb.cc
        message/message.pb.cc
        message/proxymessage.pb.h
        message/proxymessage.pb.cc
        message/configure.pb.h
        message/configure.pb.cc
        )

include_directories(
        /usr/local/include
)

link_directories(
        /usr/local/lib
        ${CMAKE_SOURCE_DIR}/lib
)

add_library(common ${COMMON})
target_link_libraries(common pthread protobuf)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
