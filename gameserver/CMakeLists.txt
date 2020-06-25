cmake_minimum_required(VERSION 3.6)
project(gameserver)
add_compile_options(-Wall -ggdb -pipe -std=c++14)
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
        inc/message_dispatcher.h
        src/message_dispatcher.cpp
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
        ${CMAKE_SOURCE_DIR}/gameserver/inc
        ${CMAKE_SOURCE_DIR}/gameserver/gamemodule/datamodule/
        ${CMAKE_SOURCE_DIR}/gameserver/gamemodule/logicmodule/
        ${CMAKE_SOURCE_DIR}/gameserver/gamemodule/metamodule/
)

link_directories(
        /usr/local/lib/
        ${CMAKE_SOURCE_DIR}/lib
)

add_executable(gameserver ${SOURCE_FILES})
target_link_libraries(gameserver framework pthread protobuf crypto)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)