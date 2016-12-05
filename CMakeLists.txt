cmake_minimum_required(VERSION 3.6)
add_definitions(-g)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(server)
add_subdirectory(common)
add_subdirectory(proxyserver)
add_subdirectory(tcpserver)

