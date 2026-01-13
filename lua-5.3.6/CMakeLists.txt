cmake_minimum_required(VERSION 3.6)
project(lua)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    add_compile_options(-O2 -Wall -g -pipe -Wextra -std=c++11)
    set(CMAKE_CXX_STANDARD 11)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_CXX_STANDARD 11)
endif()

# 仅对 Visual Studio 编译器生效
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

file(GLOB_RECURSE LUA_HEADER_FILES 
        "*.h")
file(GLOB_RECURSE LUA_SOURCE_FILES 
        "*.c")

#用c++编译器重新编译lua https://blog.codingnow.com/2015/05/lua_c_api.html(如何正确使用lua api)
set_source_files_properties(${LUA_HEADER_FILES} PROPERTIES LANGUAGE CXX)
set_source_files_properties(${LUA_SOURCE_FILES} PROPERTIES LANGUAGE CXX)

include_directories(
        /usr/include/
        /usr/local/include/
)

link_directories(
        /usr/lib/
        /usr/local/lib/
)

add_library(lua
        ${LUA_HEADER_FILES}
        ${LUA_SOURCE_FILES})

target_link_libraries(lua)