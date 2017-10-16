cmake_minimum_required(VERSION 3.6)
project(dbeserver)
add_definitions(-D_DEBUG_ -DUSE_LOG4CPP -DUSE_MY_ASSERT -DLINUX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -pipe -std=c++14")

set(SOURCE_FILES
        main.cpp
        inc/dbhandle.h
        src/dbhandle.cpp
        src/dbctrl.cpp
        inc/dbctrl.h
        inc/basedb.h
        inc/database.h
        inc/databasemysql.h
        inc/field.h
        inc/messagefactory.h
        inc/queryresult.h
        inc/queryresultmysql.h
        src/database.cpp
        src/databasemysql.cpp
        src/field.cpp
        src/messagefactory.cpp
        src/queryresultmysql.cpp)

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
add_executable(dbeserver ${SOURCE_FILES})
target_link_libraries(dbeserver framework protobuf crypto mysqlclient)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)