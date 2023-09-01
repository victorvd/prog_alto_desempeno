cmake_minimum_required(VERSION 3.5)

project(MPI_mult C)

add_executable(MPI_mult MPI_mult.c)

# Require MPI for this project:
find_package(MPI REQUIRED)
target_link_libraries(MPI_mult  PUBLIC MPI::MPI_C)

set(CMAKE_C_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILE_FLAGS ${CMAKE_C_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_C_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} ${MPI_LINK_FLAGS})
