cmake_minimum_required(VERSION 3.6)
project(dbserver)
add_definitions(-D_DEBUG_ -DUSE_LOG4CPP -DUSE_MY_ASSERT -DLINUX)

set(SOURCE_FILES
        main.cpp
        src/dbctrl.cpp
        inc/dbctrl.h
        inc/basedb.h
        inc/database.h
        inc/database_mysql.h
        inc/field.h
        inc/message_factory.h
        inc/query_result.h
        inc/query_result_mysql.h
        src/database.cpp
        src/database_mysql.cpp
        src/field.cpp
        src/message_factory.cpp
        src/query_result_mysql.cpp)

include_directories(
        /usr/include/
        /usr/local/include/
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/const
        ${CMAKE_SOURCE_DIR}/framework/event
        ${CMAKE_SOURCE_DIR}/framework/include
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
add_executable(dbserver ${SOURCE_FILES})
target_link_libraries(dbserver framework protobuf crypto mysqlclient)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)