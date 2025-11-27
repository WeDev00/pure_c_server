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
        headers/controller/english_controller.h
        headers/controller/italian_controller.h
        headers/utility/utility.h
        src/utility/utility.c
        src/db/db.c
        headers/controller/generic_controller.h
        headers/repository/italian_repository.h
        headers/repository/english_repository.h
        src/repository/english_repository.c
        src/repository/italian_repository.c)

target_link_libraries(pure_c_server ws2_32 libpq)