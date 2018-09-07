cmake_minimum_required(VERSION 3.9)
project(CS460)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD gcc)
set(CMAKE_C_FLAGS -m32)
set(CMAKE_CXX_COMPILER gcc)
set(CMAKE_CXX_FLAGS -m32)

add_executable(CS460Lab2 main.c)