cmake_minimum_required(VERSION 3.6)
project(framework)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_CXX_STANDARD 11)
    add_compile_options(-Wall -fPIC -ggdb -pipe -std=c++11)

    add_definitions(-D__LINUX__ )
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_CXX_STANDARD 11)
    add_definitions(-D__WINDOWS__)
endif()

set(FRAMEWORK
        "base/base.cpp"
        "base/base.h"
        "base/my_assert.h"
        "base/my_macro.h" 
        "base/object.h"
        "base/object_manager.cpp"
        "base/object_manager.h"
        "base/performance.cpp"
        "base/performance.h"
        "base/reflection.h"
        "base/server_tool.cpp"
        "base/server_tool.h"
        "base/platform_def.h" 
        "base/t_array.h"
        "base/safe_pointer.h" 
        "base/rb_tree.h"
        "base/hash_function.h"
        "base/hash_helper.h"
        "base/rbthash_map.h"
        "const/common_def.h"
        "const/dblogin_type.h"
        "const/err_code.h"
        "json/config.cpp"
        "json/config.h"
        "json/json_interface.cpp"
        "json/json_interface.h"
        "log/log.cpp" 
        "log/log.h"
        "mem/shm_queue.cpp" 
        "mem/shm_queue.h"
        "mem/mempool_safty.h" 
        "mem/mem_pool.h"
        "mem/shm_api.cpp"
        "mem/shm_api.h" 
        "mem/shm.cpp"
        "mem/shm.h"
        "../common/message/common.pb.h" 
        "../common/message/dbmessage.pb.h"
        "../common/message/message.pb.h"
        "../common/message/player.pb.h"
        "net/acceptor.cpp" 
        "net/acceptor.h"
        "net/buffev_interface.cpp" 
        "net/buffev_interface.h"
        "net/byte_buff.cpp" 
        "net/byte_buff.h" 
        "net/ccrypto.cpp" 
        "net/ccrypto.h"
        "net/client_comm_engine.cpp" 
        "net/client_comm_engine.h"
        "net/connector.cpp" 
        "net/connector.h" 
        "net/event_reactor.cpp" 
        "net/event_reactor.h"
        "net/file_listener.cpp"
        "net/file_listener.h"
        "net/listener.cpp" 
        "net/listener.h"
        "net/message_interface.cpp"
        "net/message_interface.h" 
        "net/network_interface.h" 
        "net/net_addr.h"
        "net/net_inc.h"
        "net/net_work.cpp"
        "net/net_work.h"
        "net/runflag.cpp" 
        "net/runflag.h"
        "net/server_comm_engine.cpp"
        "net/server_comm_engine.h" 
        "net/socket.cpp"
        "net/socket.h" 
        "net/statistics.cpp"
        "net/statistics.h" 
        "net/system_signal.cpp"
        "net/system_signal.h" 
        "net/timer_event.cpp" 
        "net/timer_event.h" 
        "thread/mythread.cpp"
        "thread/mythread.h" 
        "thread/thread_pool.cpp"
        "thread/thread_pool.h" 
        "timer/clock.cpp" 
        "timer/clock.h"
        "timer/timer.cpp"
        "timer/timer.h")


include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/common/message
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/const
        ${CMAKE_SOURCE_DIR}/framework/json
        ${CMAKE_SOURCE_DIR}/framework/log
        ${CMAKE_SOURCE_DIR}/framework/mem
        ${CMAKE_SOURCE_DIR}/framework/message
        ${CMAKE_SOURCE_DIR}/framework/net
        ${CMAKE_SOURCE_DIR}/framework/thread
        ${CMAKE_SOURCE_DIR}/framework/timer
        ${CMAKE_SOURCE_DIR}/framework/hiredis
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

add_library(framework ${FRAMEWORK})

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(framework pthread spdlog event event_pthreads protobuf crypto)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(framework spdlogd event event_pthreads protobuf crypto)
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
