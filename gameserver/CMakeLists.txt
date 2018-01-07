cmake_minimum_required(VERSION 3.6)
project(gameserver)
add_definitions(-D_DEBUG_ -DUSE_LOG4CPP -DUSE_MY_ASSERT -DLINUX)

set(SOURCE_FILES
        main.cpp
        inc/gameserver.h
        inc/clienthandle.h
        inc/messagefactory.h
        inc/messagedispatcher.h
        datamodule/inc/sceneobjmanager.h
        src/gameserver.cpp
        src/clienthandle.cpp
        src/messagefactory.cpp
        src/messagedispatcher.cpp
        datamodule/src/sceneobjmanager.cpp
        datamodule/inc/player.h
        datamodule/inc/playerdata.h
        datamodule/inc/playerbase.h
        datamodule/inc/playercity.h
        datamodule/src/player.cpp
        datamodule/src/playerbase.cpp
        datamodule/src/playerdata.cpp
        datamodule/src/playercity.cpp
        logicmodule/inc/coremodule.h
        logicmodule/inc/logicmodule.h
        logicmodule/inc/modulemanager.h
        logicmodule/inc/dbmodule.h
        logicmodule/src/modulemanager.cpp
        logicmodule/src/coremodule.cpp
        logicmodule/src/logicmodule.cpp
        logicmodule/src/dbmodule.cpp
        metadata/inc/parameter_config.h
        metadata/src/parameter_config.cpp
        metadata/inc/parameter_key.h
        )

include_directories(
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
)

link_directories(
        /usr/local/lib/
        ${CMAKE_SOURCE_DIR}/lib
)

add_executable(gameserver ${SOURCE_FILES})
target_link_libraries(gameserver framework pthread protobuf )
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)