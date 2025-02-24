cmake_minimum_required(VERSION 3.10.0)
project(hello VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

file(GLOB SOURCES "*.c")

add_executable(hello ${SOURCES})

