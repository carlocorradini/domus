cmake_minimum_required(VERSION 3.13)
project(Domus C)

set(CMAKE_C_STANDARD 11)

add_executable(Domus main.c color.h cli.h cli.c color.c domus.h domus.c author.h)