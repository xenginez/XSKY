cmake_minimum_required (VERSION 3.8)

project ("XSKY")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
add_subdirectory("route")
add_subdirectory("server")
else()
add_subdirectory("control")
add_subdirectory("tests")

if(LINUX_KNL)
add_subdirectory("route")
endif()

endif()