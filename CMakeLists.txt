cmake_minimum_required(VERSION 3.6)
add_definitions(-Wall -DUSE_LOG4CPP -g -std=c++11)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(server)
add_subdirectory(framework)
add_subdirectory(gateserver)
add_subdirectory(proxyserver)
add_subdirectory(dbserver)
add_subdirectory(gameserver)


