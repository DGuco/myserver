cmake_minimum_required(VERSION 3.6)
add_definitions(-Wall -g -std=c++11)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(server)
add_subdirectory(framework)
add_subdirectory(tcpserver)
add_subdirectory(gateserver)


