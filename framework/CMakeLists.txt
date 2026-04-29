cmake_minimum_required(VERSION 3.6)
project(framework)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    add_compile_options(-Wall -fPIC -ggdb -pipe -std=c++11)
    set(CMAKE_CXX_STANDARD 11)
    add_definitions(-D__LINUX__ -DSPDLOG_NO_NAME -DCOMPILE_LUA_WITH_CXX)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_CXX_STANDARD 11)
    add_definitions(-D__WINDOWS__ -D_DEBUG_ -DWIN32_LEAN_AND_MEAN -DSPDLOG_NO_NAME -DCOMPILE_LUA_WITH_CXX)
endif()

# 仅对 Visual Studio 编译器生效
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

set(FRAMEWORK
        "base/base.h"
        "base/platform_def.h" 
        "base/platform_def.cpp"
        "base/my_assert.h"
        "base/object.h"
        "base/performance.cpp"
        "base/performance.h"
        "base/reflection.h"
        "base/time_helper.h"
        "base/time_helper.cpp"
        "base/log.h"
        "base/log.cpp"
        "base/signal_handler.h"
        "base/signal_handler.cpp"
        "base/message_factory.h"
        "base/message_factory.cpp" 
        "base/base_header.h"
        "std/t_array.h"
        "std/rb_tree.h"
        "std/hash_function.h"
        "std/hash_helper.h"
        "std/rbthash_map.h"
        "std/safe_pointer.h"
        "std/my_new.h"
        "std/my_new.cpp"
        "std/c_string.h"
        "shm/shm_queue.h"
        "shm/shm_queue.cpp" 
        "shm/shm_api.cpp"
        "shm/shm_api.h" 
        "shm/shm.cpp"
        "shm/shm.h"
        "shm/shm_pool.h"
        "net/statistics.h" 
        "net/statistics.cpp"
        "net/byte_buff.cpp" 
        "net/byte_buff.h" 
        "net/ccrypto.cpp" 
        "net/ccrypto.h"
        "net/runflag.cpp" 
        "net/runflag.h"
        "net/socket.cpp"
        "net/socket.h" 
        "net/tcp_socket.h"
        "net/tcp_socket.cpp" 
        "net/tcp_server.h" 
        "net/tcp_server.cpp" 
        "net/tcp_conn.h" 
        "net/tcp_conn.cpp"
        "net/tcp_client.h"
        "net/tcp_client.cpp"
        "thread/my_thread.h" 
        "thread/my_thread.cpp"
        "thread/thread_scheduler.h" 
        "thread/thread_scheduler.cpp"
        "thread/task.h" 
        "thread/task.cpp"
        "thread/my_lock.h" 
        "thread/task_helper.h"
        "thread/spin_lock.h"
        "thread/task_scheduler.h"
        "thread/task_scheduler.cpp"
        "db/basedb.h"
        "db/database.h"
        "db/database_mysql.h"
        "db/field.h"
        "db/query_result.h"
        "db/query_result_mysql.h"
        "db/database_mysql.cpp"
        "db/database.cpp"
        "db/field.cpp"
        "db/query_result_mysql.cpp")

file(GLOB_RECURSE LUA_BRIDGE_HEADER_FILES 
        "lua/core/*.h"
        "lua/lua_file.h"
        "lua/lua_bridge.h"
        )

list(APPEND FRAMEWORK ${LUA_BRIDGE_HEADER_FILES})

include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/common/message
        ${CMAKE_SOURCE_DIR}/lua-5.3.6
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/mem
        ${CMAKE_SOURCE_DIR}/framework/message
        ${CMAKE_SOURCE_DIR}/framework/net
        ${CMAKE_SOURCE_DIR}/framework/thread
        ${CMAKE_SOURCE_DIR}/framework/std
        ${CMAKE_SOURCE_DIR}/framework/db
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

add_library(framework ${LUA_BRIDGE_HEADER_FILES} ${FRAMEWORK})

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(framework pthread spdlog protobuf lua)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(framework ws2_32 spdlogd protobufd lua)
endif()

