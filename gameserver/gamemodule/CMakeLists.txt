cmake_minimum_required(VERSION 3.6)
project(gamemodule)

add_definitions(-D_DEBUG_ -DUSE_MY_ASSERT -DLOG_THREAD_SAFE)
add_compile_options(-fPIC)

set(GAMEMODULE
        datamodule/inc/sceneobjmanager.h
        datamodule/inc/player.h
        datamodule/inc/playerdata.h
        datamodule/inc/playerbase.h
        datamodule/inc/playercity.h
        datamodule/src/sceneobjmanager.cpp
        datamodule/src/player.cpp
        datamodule/src/playerbase.cpp
        datamodule/src/playerdata.cpp
        datamodule/src/playercity.cpp
        logicmodule/inc/core_module.h
        logicmodule/inc/logic_module.h
        logicmodule/inc/module_manager.h
        logicmodule/inc/db_module.h
        logicmodule/src/module_manager.cpp
        logicmodule/src/core_module.cpp
        logicmodule/src/logic_module.cpp
        logicmodule/src/db_module.cpp
        metamodule/inc/parameter_config.h
        metamodule/src/parameter_config.cpp)

include_directories(
        /usr/include
        /usr/local/include
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/const
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
        /usr/local/lib
        ${CMAKE_SOURCE_DIR}/lib
)

add_library(gamemodule SHARED ${GAMEMODULE})
target_link_libraries(gamemodule framework)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
