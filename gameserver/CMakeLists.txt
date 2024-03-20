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

set(SOURCE_FILES
        main.cpp
        inc/game_server.h
        inc/client_handle.h
        inc/message_factory.h
        inc/server_handle.h
        inc/game_ctrl.h
        inc/config_handle.h
        inc/message_dispatcher.h
        inc/game_player.h
        src/game_ctrl.cpp
        src/server_handle.cpp
        src/game_server.cpp
        src/client_handle.cpp
        src/message_factory.cpp
        src/config_handle.cpp
        src/message_dispatcher.cpp
        src/game_player.cpp
        gamemodule/datamodule/sceneobjmanager.h
        gamemodule/datamodule/player.h
        gamemodule/datamodule/playerdata.h
        gamemodule/datamodule/playerbase.h
        gamemodule/datamodule/playercity.h
        gamemodule/datamodule/sceneobjmanager.cpp
        gamemodule/datamodule/player.cpp
        gamemodule/datamodule/playerbase.cpp
        gamemodule/datamodule/playerdata.cpp
        gamemodule/datamodule/playercity.cpp
        gamemodule/logicmodule/core_module.h
        gamemodule/logicmodule/logic_module.h
        gamemodule/logicmodule/module_manager.h
        gamemodule/logicmodule/db_module.h
        gamemodule/logicmodule/module_manager.cpp
        gamemodule/logicmodule/core_module.cpp
        gamemodule/logicmodule/logic_module.cpp
        gamemodule/logicmodule/db_module.cpp
        gamemodule/metamodule/parameter_config.h
        gamemodule/metamodule/parameter_config.cpp
        )

include_directories(
        ${CMAKE_SOURCE_DIR}/thirdparty
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/message
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/const
        ${CMAKE_SOURCE_DIR}/framework/json
        ${CMAKE_SOURCE_DIR}/framework/log
        ${CMAKE_SOURCE_DIR}/framework/mem
        ${CMAKE_SOURCE_DIR}/framework/net
        ${CMAKE_SOURCE_DIR}/framework/thread
        ${CMAKE_SOURCE_DIR}/gameserver/inc
        ${CMAKE_SOURCE_DIR}/gameserver/gamemodule/datamodule/
        ${CMAKE_SOURCE_DIR}/gameserver/gamemodule/logicmodule/
        ${CMAKE_SOURCE_DIR}/gameserver/gamemodule/metamodule/
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
    target_link_libraries(gameserver framework ws2_32 protobuf crypto)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run/)