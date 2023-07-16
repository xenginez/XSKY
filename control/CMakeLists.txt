cmake_minimum_required (VERSION 3.8)

add_executable (control "main.cpp" "control.h" "control.cpp")
target_include_directories(control PRIVATE ${CMAKE_SOURCE_DIR}/3rdparty)
target_compile_definitions(control PRIVATE -DCONFIG_IPV6)
 