cmake_minimum_required(VERSION 3.13)
project(Domus C)

set(CMAKE_C_STANDARD 11)

#Set directories.  The dot representes the root application folder.
set(HEADER ./include)
set(SOURCE ./src)

#Include the directories
include_directories(${HEADER})
include_directories(${SOURCE})

set(
        SOURCE_FILES
        main.c
        ${HEADER}/color.h
        ${SOURCE}/color.c
        ${HEADER}/author.h
        ${SOURCE}/author.c
        ${HEADER}/domus.h
        ${SOURCE}/domus.c
        ${HEADER}/cli.h
        ${SOURCE}/cli.c
)

add_executable(Domus ${SOURCE_FILES})