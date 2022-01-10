cmake_minimum_required(VERSION 3.21)
project(c_http C)

set(CMAKE_C_STANDARD 99)

add_library(c_http library.c)
add_executable(c_http_exec main.c library.c)
