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
        ./project/include/cli/cli.h
        ./project/src/cli/cli.c
        ./project/include/domus.h
        ./project/src/domus.c
        ./project/include/printer.h
        ./project/src/printer.c
        ./project/include/cli/command/command.h
        ./project/src/cli/command/command.c
        ./project/include/cli/command/command_exit.h
        ./project/src/cli/command/command_exit.c
        ./project/include/cli/command/command_help.h
        ./project/src/cli/command/command_help.c
        ./project/include/cli/command/command_list.h
        ./project/src/cli/command/command_list.c
        ./project/include/device/device.h
        ./project/src/device/device.c
        project/include/device/control/device_controller.h
        project/src/device/control/device_controller.c
        project/include/device/control/device_hub.h
        project/src/device/control/device_hub.c
        project/include/device/control/device_timer.h
        project/src/device/control/device_timer.c
        project/src/cli/command/command_clear.c
        project/include/cli/command/command_add.h project/include/cli/command/command_del.h project/include/cli/command/command_link.h project/include/cli/command/command_switch.h project/include/cli/command/command_info.h project/src/cli/command/command_add.c project/src/cli/command/command_del.c project/src/cli/command/command_info.c project/src/cli/command/command_link.c project/src/cli/command/command_switch.c project/include/collection/collection_list.h project/src/collection/collection_list.c)

add_executable(domus ${SOURCE_FILES})