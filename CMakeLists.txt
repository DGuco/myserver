cmake_minimum_required(VERSION 3.6)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
	message("current platform: Linux ")
	set(CMAKE_C_COMPILER gcc)
	set(CMAKE_CXX_COMPILER g++)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
	message("current platform: Windows ")
endif()



project(server)
add_subdirectory(framework)
add_subdirectory(gateserver)
add_subdirectory(proxyserver)
add_subdirectory(dbserver)
add_subdirectory(gameserver)

