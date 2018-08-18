cmake_minimum_required(VERSION 3.6)
project(test)
add_definitions(-D_DEBUG_ -DUSE_MY_ASSERT)

set(SOURCE_FILES
        main.cpp
        )

include_directories(
        /usr/include/
        /usr/local/include/
)

link_directories(
        /usr/local/lib/
        ${CMAKE_SOURCE_DIR}/lib
)

add_executable(demo ${SOURCE_FILES})
target_link_libraries(demo test1so test2so protobuf crypto)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)