cmake_minimum_required(VERSION 3.6)
project(gameserver)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    add_compile_options(-Wall -ggdb -pipe -std=c++14)

    add_definitions(-D_LINUX_ -D_DEBUG_ -DSPDLOG_NO_NAME)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-D__WINDOWS__ -D_DEBUG_ -DSPDLOG_NO_NAME)
endif()

# ���� Visual Studio ��������Ч
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

set(SOURCE_FILES
        "main.cpp"
        "../common/message/message.pb.cc" 
        "../common/message/message.pb.h"
        "../common/common_def.h"
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
        "gamemodule/datamodule/playerdata.h"
        "gamemodule/datamodule/playerbase.h"
        "gamemodule/datamodule/playercity.h"
        "gamemodule/datamodule/player.cpp"
        "gamemodule/datamodule/playerbase.cpp"
        "gamemodule/datamodule/playerdata.cpp"
        "gamemodule/datamodule/playercity.cpp"
        "gamemodule/messagemodule/cgconn_req.h"
        )

include_directories(
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/common/message
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/const
        ${CMAKE_SOURCE_DIR}/framework/log
        ${CMAKE_SOURCE_DIR}/framework/net
        ${CMAKE_SOURCE_DIR}/framework/thread
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
        ${CMAKE_SOURCE_DIR}/lib/linux
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        C:/Windows/System32
        ${CMAKE_SOURCE_DIR}/lib/win64
endif()
)

add_executable(gameserver ${SOURCE_FILES})

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(gameserver framework pthread protobuf crypto)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(gameserver framework ws2_32 protobufd crypto)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run/)