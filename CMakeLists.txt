cmake_minimum_required (VERSION 3.8)

project ("sky")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
add_subdirectory("driver")
add_subdirectory("control")
else()
add_subdirectory("interface")
endif()