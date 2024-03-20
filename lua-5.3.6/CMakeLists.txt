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

set(LUA_HEADER_FILES
        lapi.h
        lauxlib.h
        lcode.h
        lctype.h
        ldebug.h
        ldo.h
        lfunc.h
        lgc.h
        llex.h
        llimits.h
        lmem.h
        lobject.h
        lopcodes.h
        lparser.h
        lprefix.h
        lstate.h
        lstring.h
        ltable.h
        ltm.h
        lua.h
        luaconf.h
        lualib.h
        lundump.h
        lvm.h
        lzio.h)

set(LUA_SOURCE_FILES
        lapi.c
        lauxlib.c
        lbaselib.c
        lbitlib.c
        lcode.c
        lcorolib.c
        lctype.c
        ldblib.c
        ldebug.c
        ldo.c
        ldump.c
        lfunc.c
        lgc.c
        linit.c
        liolib.c
        llex.c
        lmathlib.c
        lmem.c
        loadlib.c
        lobject.c
        lopcodes.c
        loslib.c
        lparser.c
        lstate.c
        lstring.c
        lstrlib.c
        ltable.c
        ltablib.c
        ltm.c
        lundump.c
        lutf8lib.c
        lvm.c
        lzio.c
        )

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

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)