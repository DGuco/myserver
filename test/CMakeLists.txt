cmake_minimum_required(VERSION 3.6)
project(test)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    add_compile_options(-Wall -ggdb -pipe -std=c++11)

    add_definitions(-D__LINUX__ -DSPDLOG_NO_NAME -DCOMPILE_LUA_WITH_CXX)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-D__WINDOWS__ -D_DEBUG_ -DWIN32_LEAN_AND_MEAN -DSPDLOG_NO_NAME -DCOMPILE_LUA_WITH_CXX)
endif()

# 仅对 Visual Studio 编译器生效
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

set(SOURCE_FILES
    "main.cpp" 
    "lock_free_limit_queue_test.h"
    "rbt_hash_map_test.h"
    "atomic_test.h"
    "scheduler_test.h"
    "test_common/FB_BaseType.h"
    "test_common/FB_BaseType.cpp"
    "test_common/db_struct.h"
    "homelogic/homeplant_config.h"
    "homelogic/homeplant_config.cpp"
    "homelogic/homeplant_manager.h"
    "homelogic/homeplant_manager.cpp"
    "homelogic/cgaskhomeplant.h"
    "homelogic/cgaskhomeplant.cpp"
    "homelogic/cgaskhomeplanthandler.cpp"
    "homelogic/gcrethomeplant.h"
    "homelogic/gcrethomeplant.cpp"
    "homelogic/gwghomeplantopt.h"
    "homelogic/gwghomeplantopt.cpp"
    "homelogic/gwghomeplantopthandler.cpp"
    )


include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/common/message
        ${CMAKE_SOURCE_DIR}/lua-5.3.6/
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/shm
        ${CMAKE_SOURCE_DIR}/framework/message
        ${CMAKE_SOURCE_DIR}/framework/net
        ${CMAKE_SOURCE_DIR}/framework/thread
        ${CMAKE_SOURCE_DIR}/framework/std
        ${CMAKE_SOURCE_DIR}/framework/lua
        ${CMAKE_SOURCE_DIR}/framework/db
        ${CMAKE_SOURCE_DIR}/test/test_common
        ${CMAKE_SOURCE_DIR}/test/homelogic

)

link_directories(
        ${CMAKE_SOURCE_DIR}/lib
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        /usr/lib
        /usr/local/lib
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/lib/linux
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        C:/Windows/System32
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/lib/win64
        ${CMAKE_SOURCE_DIR}/lib/Debug
endif()
)

add_executable(test ${SOURCE_FILES})
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(test framework)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(test framework psapi)
endif()

