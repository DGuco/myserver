cmake_minimum_required(VERSION 3.6)
project(dbserver)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    add_compile_options(-Wall -ggdb -pipe -std=c++11)

    add_definitions(-D__LINUX__ -D_DEBUG_ -DSPDLOG_NO_NAME)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    #DWIN32_LEAN_AND_MEAN error C2375: “WSACleanup”: 重定义；不同的链接等
    #https://blog.csdn.net/c_base_jin/article/details/93391286
    add_definitions(-D__WINDOWS__ -DWIN32_LEAN_AND_MEAN -D_DEBUG_ -DSPDLOG_NO_NAME)
endif()

# 仅对 Visual Studio 编译器生效
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/common/tcp_def.h
    ${CMAKE_SOURCE_DIR}/common/server_config.h
    ${CMAKE_SOURCE_DIR}/common/server_config.cpp
    ${CMAKE_SOURCE_DIR}/common/db/db_playercity.h
    ${CMAKE_SOURCE_DIR}/common/db/db_playerbase.h
    ${CMAKE_SOURCE_DIR}/common/db/db_playerdata.h
    ${CMAKE_SOURCE_DIR}/common/db/shm_def.h
    "main.cpp"
    "inc/db_server.h"
    "inc/db_ctrl.h"
    "inc/db_client.h"
    "inc/shm_manager.h"
    "src/db_server.cpp"
    "src/db_ctrl.cpp"
    "src/db_client.cpp"
    "src/shm_manager.cpp"
    )

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/common/db
    ${CMAKE_SOURCE_DIR}/common/message
    ${CMAKE_SOURCE_DIR}/lua-5.3.6/
    ${CMAKE_SOURCE_DIR}/thirdparty
    ${CMAKE_SOURCE_DIR}/framework/base
    ${CMAKE_SOURCE_DIR}/framework/shm
    ${CMAKE_SOURCE_DIR}/framework/net
    ${CMAKE_SOURCE_DIR}/framework/thread
    ${CMAKE_SOURCE_DIR}/framework/std
    ${CMAKE_SOURCE_DIR}/framework/lua
    ${CMAKE_SOURCE_DIR}/framework/db
    ${CMAKE_SOURCE_DIR}/dbserver/inc
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
endif()
)

add_executable(dbserver ${SOURCE_FILES})

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(dbserver framework libmysql)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(dbserver framework libmysql)
endif()
