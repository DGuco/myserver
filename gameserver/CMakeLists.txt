cmake_minimum_required(VERSION 3.6)
project(gameserver)
add_definitions(-D_DEBUG_ -DUSE_LOG4CPP -DUSE_MY_ASSERT)

set(SOURCE_FILES
        main.cpp
        inc/game_server.h
        inc/client_handle.h
        inc/message_factory.h
        inc/server_handle.h
        src/server_handle.cpp
        src/game_server.cpp
        src/client_handle.cpp
        src/message_factory.cpp
        inc/config_handle.h
        src/config_handle.cpp
        gamemodule/datamodule/inc/sceneobjmanager.h
        gamemodule/datamodule/inc/player.h
        gamemodule/datamodule/inc/playerdata.h
        gamemodule/datamodule/inc/playerbase.h
        gamemodule/datamodule/inc/playercity.h
        gamemodule/datamodule/src/sceneobjmanager.cpp
        gamemodule/datamodule/src/player.cpp
        gamemodule/datamodule/src/playerbase.cpp
        gamemodule/datamodule/src/playerdata.cpp
        gamemodule/datamodule/src/playercity.cpp
        gamemodule/logicmodule/inc/core_module.h
        gamemodule/logicmodule/inc/logic_module.h
        gamemodule/logicmodule/inc/module_manager.h
        gamemodule/logicmodule/inc/db_module.h
        gamemodule/logicmodule/src/module_manager.cpp
        gamemodule/logicmodule/src/core_module.cpp
        gamemodule/logicmodule/src/logic_module.cpp
        gamemodule/logicmodule/src/db_module.cpp
        gamemodule/metamodule/inc/parameter_config.h
        gamemodule/metamodule/src/parameter_config.cpp
        )

include_directories(
        /usr/include/
        /usr/local/include/
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/const
        ${CMAKE_SOURCE_DIR}/framework/event
        ${CMAKE_SOURCE_DIR}/framework/json
        ${CMAKE_SOURCE_DIR}/framework/log
        ${CMAKE_SOURCE_DIR}/framework/mem
        ${CMAKE_SOURCE_DIR}/framework/message
        ${CMAKE_SOURCE_DIR}/framework/net
        ${CMAKE_SOURCE_DIR}/framework/thread
        ${CMAKE_SOURCE_DIR}/framework/timer
        ${CMAKE_SOURCE_DIR}/gameserver/gamemodule/datamodule/inc
        ${CMAKE_SOURCE_DIR}/gameserver/gamemodule/logicmodule/inc
        ${CMAKE_SOURCE_DIR}/gameserver/gamemodule/metamodule/inc
)

link_directories(
        /usr/local/lib/
        ${CMAKE_SOURCE_DIR}/lib
)

add_executable(gameserver ${SOURCE_FILES})
target_link_libraries(gameserver framework pthread protobuf crypto)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)