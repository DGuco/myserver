cmake_minimum_required(VERSION 3.6)
project(framework)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_CXX_STANDARD 11)
    add_compile_options(-Wall -fPIC -ggdb -pipe -std=c++11)
    
    set(CMAKE_CXX_STANDARD 11)
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
        "const/platform_def.h" 
        "base/t_array.h"
        "base/safe_pointer.h" 
        "base/rb_tree.h"
        "base/hash_function.h"
        "base/hash_helper.h"
        "base/rbthash_map.h"
        "base/time_helper.h"
        "base/time_helper.cpp"
        "const/dblogin_type.h"
        "const/err_code.h"
        "json/config.cpp"
        "json/config.h"
        "json/json_interface.cpp"
        "json/json_interface.h"
        "log/log.h"
        "log/log.cpp" 
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
        "net/message_interface.cpp"
        "net/message_interface.h" 
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
        "net/tcp_client.cpp")


include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/common
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
    target_link_libraries(framework pthread spdlog protobuf crypto)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(framework ws2_32 protobuf crypto )
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
