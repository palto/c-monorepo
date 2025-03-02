cmake_minimum_required(VERSION 3.10.0)
project(hello VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

# This enables CMake to locate sqlite3
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_library(utils utils/trim.c utils/prompt.c)

add_executable(fibonacci fibonacci/fibonacci.c)
add_executable(server server/hello_server.c)


target_include_directories(server PRIVATE utils)
target_include_directories(fibonacci PRIVATE utils)
target_link_libraries(server PRIVATE utils unofficial::sqlite3::sqlite3)
target_link_libraries(fibonacci utils)