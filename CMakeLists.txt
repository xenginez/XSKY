cmake_minimum_required (VERSION 3.8)

project ("XSKY")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_subdirectory("route")
	add_subdirectory("server")
else()
	add_subdirectory("client")
endif()

	add_subdirectory("controller")