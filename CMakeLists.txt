cmake_minimum_required(VERSION 4.0)
project(pure_c_server C)

set(CMAKE_C_STANDARD 11)

add_executable(pure_c_server src/server.c
        src/handlers/gateway.c
        src/handlers/headers/gateway.h
        src/handlers/controller/english_controller.c
        src/handlers/controller/italian_controller.c
        src/handlers/headers/controller_headers/english_controller.h
        src/handlers/headers/controller_headers/italian_controller.h
        src/utility/utility.h
        src/utility/utility.c)

target_link_libraries(pure_c_server ws2_32)