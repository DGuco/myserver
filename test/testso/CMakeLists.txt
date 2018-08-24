cmake_minimum_required(VERSION 3.6)
project(.)

add_definitions(-D_DEBUG_ -DUSE_MY_ASSERT -DLOG_THREAD_SAFE)
add_compile_options(-shared -fPIC)

set(TESTSO
        singleton.h
        singleton.cpp
        test.cpp)

include_directories(
        /usr/include
        /usr/local/include)

link_directories(
        /usr/local/lib
        ${CMAKE_SOURCE_DIR}/lib
)


add_library(test1so SHARED ${TESTSO})
target_link_libraries(test1so)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
