cmake_minimum_required (VERSION 3.8)
project(controller LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable (controller "main.cpp" "device.cpp" "controller.cpp" "task.h" "task.cpp" "tasks/tasks.h" "tasks/tcp_task.h" "tasks/tcp_task.cpp" "tasks/udp_task.h" "tasks/udp_task.cpp" "tasks/dns_task.h" "tasks/dns_task.cpp" "tasks/http_task.h" "tasks/http_task.cpp" "tasks/https_task.h" "tasks/https_task.cpp" "tasks/https3_task.cpp" "tasks/https3_task.h" "apnic.h" "apnic.cpp" "mmap.h" "mmap.cpp")
target_include_directories(controller PRIVATE ../3rdparty/concurrentqueue ../3rdparty/asio/asio/include ../common ./)