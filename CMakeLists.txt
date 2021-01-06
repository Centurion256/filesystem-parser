cmake_minimum_required(VERSION 3.0.0)
project(fsparse LANGUAGES C VERSION 0.1.0)
set(CMAKE_C_STANDARD 99)

add_library(vector STATIC src/vector.c inc/vector.h)
target_include_directories(vector PUBLIC inc)

add_executable(fat16parse src/fat16.c inc/fat16.h)
target_include_directories(fat16parse PUBLIC inc)
target_link_libraries(fat16parse vector)