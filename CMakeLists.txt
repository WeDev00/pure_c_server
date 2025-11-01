cmake_minimum_required(VERSION 4.0)
project(pure_c_server C)

set(CMAKE_C_STANDARD 11)

add_executable(pure_c_server src/server.c)

target_link_libraries(pure_c_server ws2_32)