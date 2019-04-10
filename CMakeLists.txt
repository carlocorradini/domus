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
        ${HEADER}/printer.h
        ${SOURCE}/printer.c
        ${HEADER}/author.h
        ${SOURCE}/author.c
        ${HEADER}/domus.h
        ${SOURCE}/domus.c
        ${HEADER}/cli.h
        ${SOURCE}/cli.c
        ${HEADER}/command/command.h src/command/command.c include/data_structure/list.h src/data_structure/list.c include/command/exit.h include/command/help.h)

add_executable(Domus ${SOURCE_FILES})