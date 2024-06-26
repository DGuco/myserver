cmake_minimum_required(VERSION 3.6)
project(framework)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_CXX_STANDARD 11)
    add_compile_options(-Wall -fPIC -ggdb -pipe -std=c++11)
    
    set(CMAKE_CXX_STANDARD 11)
    add_definitions(-D__LINUX__ -DSPDLOG_NO_NAME -DCOMPILE_LUA_WITH_CXX)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_CXX_STANDARD 11)
    add_definitions(-D__WINDOWS__ -DSPDLOG_NO_NAME -DCOMPILE_LUA_WITH_CXX)
endif()

# 仅对 Visual Studio 编译器生效
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

set(LUA_BRIDGE_HEADER_FILES
        "lua/core/func_traits.h"
        "lua/core/class_key.h"
        "lua/core/constructor.h"
        "lua/core/lua_functions.h"
        "lua/core/lua_exception.h"
        "lua/core/lua_helpers.h"
        "lua/core/lua_ref.h"
        "lua/core/lua_space.h"
        "lua/core/lua_vm.h"
        "lua/core/security.h"
        "lua/core/type_list.h"
        "lua/core/type_traits.h"
        "lua/core/user_data.h"
        "lua/core/lua_library.h"
        "lua/core/lua_stack.h"
        "lua/core/caller.h"
        "lua/core/lua_class.h"
        "lua/lua_file.h"
        "lua/lua_bridge.h")

set(FRAMEWORK
        "../common/base.h"
        "../common/dblogin_type.h"
        "../common/platform_def.h" 
        "../common/platform_def.cpp" 
        "base/my_assert.h"
        "base/object.h"
        "base/performance.cpp"
        "base/performance.h"
        "base/reflection.h"
        "base/t_array.h"
        "base/rb_tree.h"
        "base/hash_function.h"
        "base/hash_helper.h"
        "base/rbthash_map.h"
        "base/time_helper.h"
        "base/time_helper.cpp"
        "base/log.h"
        "base/log.cpp"
        "base/signal_handler.h"
        "base/signal_handler.cpp"
        "base/message_factory.h"
        "base/message_factory.cpp" 
        "base/lock_free_ring_queue.h"
        "mem/shm_queue.cpp" 
        "mem/shm_queue.h"
        "mem/mempool_safty.h" 
        "mem/mem_pool.h"
        "mem/shm_api.cpp"
        "mem/shm_api.h" 
        "mem/shm.cpp"
        "mem/shm.h"
        "thread/mythread.cpp"
        "thread/mythread.h" 
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
        "net/tcp_client.cpp"  )


include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/common/message
        ${CMAKE_SOURCE_DIR}/lua-5.3.6
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

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
