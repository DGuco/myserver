cmake_minimum_required(VERSION 3.6)
project(gameserver)
add_definitions(-D_DEBUG_ -DUSE_LOG4CPP -DUSE_MY_ASSERT)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -pipe -std=c++11")
set(SOURCE_FILES
        main.cpp
        inc/gameserver.h
        inc/clienthandle.h
        inc/messagefactory.h
        inc/messagedispatcher.h
        inc/sceneobjmanager.h
        src/gameserver.cpp
        src/clienthandle.cpp
        src/messagefactory.cpp
        src/messagedispatcher.cpp
        src/sceneobjmanager.cpp
        datamodule/inc/player.h
        datamodule/inc/playerdata.h
        datamodule/inc/playerbase.h
        datamodule/src/player.cpp
        datamodule/src/playerbase.cpp
        datamodule/src/playerdata.cpp
        logicmodule/inc/coremodule.h
        logicmodule/src/coremodule.cpp
        logicmodule/inc/logicmodule.h
        logicmodule/src/logicmodule.cpp
        logicmodule/inc/modulemanager.h
        logicmodule/src/modulemanager.cpp
        )

include_directories(
        /usr/local/include/
        ${CMAKE_SOURCE_DIR/framework/include}
        ${CMAKE_SOURCE_DIR/framework/base}
        ${CMAKE_SOURCE_DIR/framework/config}
        ${CMAKE_SOURCE_DIR/framework/log}
        ${CMAKE_SOURCE_DIR/framework/message}
        ${CMAKE_SOURCE_DIR/framework/mystd}
        ${CMAKE_SOURCE_DIR/framework/net}
        ${CMAKE_SOURCE_DIR/framework/sharemem}
        ${CMAKE_SOURCE_DIR/framework/thread}
        ${CMAKE_SOURCE_DIR/framework/timer}
)

link_directories(
        /usr/local/lib/
        ${CMAKE_SOURCE_DIR}/lib
)

add_executable(gameserver ${SOURCE_FILES})
target_link_libraries(gameserver common pthread protobuf )
set(EXECUTABgit LE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)