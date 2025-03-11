cmake_minimum_required(VERSION 3.6)
project(gameserver)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    add_compile_options(-Wall -ggdb -pipe -std=c++11)

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

set(SOURCE_FILES
        "../common/db/db_playerdata.h" 
        "../common/db/db_playercity.h"
        "../common/db/db_playerbase.h"
        "../common/message/message.pb.cc" 
        "../common/message/message.pb.h"
        "../common/tcp_def.h"
        "../common/server_config.h"
        "../common/server_config.cpp"
        "inc/game_server.h"
        "inc/game_ctrl.h"
        "inc/game_player.h"
        "inc/server_client.h"
        "inc/mfactory_manager.h"
        "src/game_ctrl.cpp"
        "src/game_server.cpp"
        "src/server_client.cpp"
        "src/game_player.cpp"
        "src/mfactory_manager.cpp"
        "gamemodule/datamodule/player.h"
        "gamemodule/datamodule/player.cpp"
        "gamemodule/messagemodule/cgconn_req.h"
        "gamemodule/messagemodule/proxy_message.h"
        "main.cpp"
         )

include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/lua-5.3.6/
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/shm
        ${CMAKE_SOURCE_DIR}/framework/message
        ${CMAKE_SOURCE_DIR}/framework/net
        ${CMAKE_SOURCE_DIR}/framework/thread
        ${CMAKE_SOURCE_DIR}/framework/std
        ${CMAKE_SOURCE_DIR}/framework/lua
        ${CMAKE_SOURCE_DIR}/gameserver/inc
        ${CMAKE_SOURCE_DIR}/gameserver/gamemodule/datamodule
        ${CMAKE_SOURCE_DIR}/gameserver/gamemodule/metamodule
        ${CMAKE_SOURCE_DIR}/gameserver/gamemodule/messagemodule
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

add_executable(gameserver ${SOURCE_FILES})

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(gameserver framework)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(gameserver framework)
endif()