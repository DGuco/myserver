cmake_minimum_required(VERSION 3.6)

project(server)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
	message("-- current platform: Linux ")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
	message("-- current platform: Windows ")
endif()


add_subdirectory(lua-5.3.6)
add_subdirectory(framework)
add_subdirectory(gameserver)
add_subdirectory(proxyserver)
add_subdirectory(dbserver)


