cmake_minimum_required(VERSION 4.0)
project(pure_c_server C)

set(CMAKE_C_STANDARD 11)
include_directories("C:/Program Files/PostgreSQL/17/include")
link_directories("C:/Program Files/PostgreSQL/17/lib")
add_executable(pure_c_server src/server.c
        src/gateway.c
        headers/gateway.h
        src/controller/english_controller.c
        src/controller/italian_controller.c
        headers/controller_headers/english_controller.h
        headers/controller_headers/italian_controller.h
        headers/utility.h
        src/utility/utility.c
        src/db/db_connection.c)

target_link_libraries(pure_c_server ws2_32 libpq)