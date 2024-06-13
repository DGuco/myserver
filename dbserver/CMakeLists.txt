cmake_minimum_required(VERSION 3.6)
project(dbserver)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    add_compile_options(-Wall -ggdb -pipe -std=c++14)

    add_definitions(-D_LINUX_ -D_DEBUG_ -DSPDLOG_NO_NAME)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-D__WINDOWS__ -D_DEBUG_ -DSPDLOG_NO_NAME)
endif()

set(SOURCE_FILES
        main.cpp
        "src/db_server.cpp"
        "inc/db_server.h"
        inc/basedb.h
        inc/database.h
        inc/database_mysql.h
        inc/field.h
        inc/query_result.h
        inc/query_result_mysql.h
        src/database.cpp
        src/database_mysql.cpp
        src/field.cpp
        src/query_result_mysql.cpp "inc/db_server.h")

include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/dbserver/inc
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/common/message
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/const
        ${CMAKE_SOURCE_DIR}/framework/json
        ${CMAKE_SOURCE_DIR}/framework/mem
        ${CMAKE_SOURCE_DIR}/framework/net
        ${CMAKE_SOURCE_DIR}/framework/thread
        ${CMAKE_SOURCE_DIR}/framework/lua/core
        ${CMAKE_SOURCE_DIR}/framework/lua
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
add_executable(dbserver ${SOURCE_FILES})

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(dbserver framework pthread protobuf crypto mysqlclient)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(dbserver framework ws2_32 protobuf crypto mysqlclient)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run/)