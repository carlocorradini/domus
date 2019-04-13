cmake_minimum_required(VERSION 3.13)
project(domus C)

set(CMAKE_C_STANDARD 90)

include_directories(./project)
include_directories(./project/src)
include_directories(./project/include)

set(
        SOURCE_FILES
        ./project/src/main.c
        ./project/include/author.h
        ./project/src/author.c
        ./project/include/cli.h
        ./project/src/cli.c
        ./project/include/domus.h
        ./project/src/domus.c
        ./project/include/printer.h
        ./project/src/printer.c
        ./project/include/collection/list.h
        ./project/src/collection/list.c
        ./project/include/command/command.h
        ./project/src/command/command.c
        ./project/include/command/exit.h
        ./project/src/command/exit.c
        ./project/include/command/help.h
        ./project/src/command/help.c
        ./project/include/device/device.h
        ./project/src/device/device.c
        ./project/include/device/control/controller.h
        ./project/src/device/control/controller.c
        ./project/include/device/control/hub.h
        ./project/src/device/control/hub.c
        ./project/include/device/control/timer.h
        ./project/src/device/control/timer.c
        )

add_executable(domus ${SOURCE_FILES})