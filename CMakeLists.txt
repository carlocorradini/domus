cmake_minimum_required(VERSION 3.13)
project(domus C)

set(CMAKE_C_STANDARD 90)

include_directories(./project)
include_directories(./project/src/src)
include_directories(./project/src/include)

set(
        SOURCE_FILES
        ./project/src/src/main.c
        ./project/src/include/author.h
        ./project/src/src/author.c
        ./project/src/include/cli/cli.h
        ./project/src/src/cli/cli.c
        ./project/src/include/domus.h
        ./project/src/src/domus.c
        project/src/include/util/util_printer.h
        project/src/src/util/util_printer.c
        ./project/src/include/cli/command/command.h
        ./project/src/src/cli/command/command.c
        ./project/src/include/cli/command/command_exit.h
        ./project/src/src/cli/command/command_exit.c
        ./project/src/include/cli/command/command_help.h
        ./project/src/src/cli/command/command_help.c
        ./project/src/include/cli/command/command_list.h
        ./project/src/src/cli/command/command_list.c
        ./project/src/include/device/device.h
        ./project/src/src/device/device.c
        ./project/src/src/collection/collection_trie.c
        project/src/include/device/control/device_controller.h
        project/src/src/device/control/device_controller.c
        project/src/include/device/control/device_hub.h
        project/src/src/device/control/device_hub.c
        project/src/include/device/control/device_timer.h
        project/src/src/device/control/device_timer.c
        project/src/src/cli/command/command_clear.c
        project/src/include/device/interaction/device_bulb.h
        project/src/src/device/interaction/device_bulb.c
        project/src/src/device/interaction/device_window.c
        project/src/include/device/interaction/device_window.h
        project/src/src/device/interaction/device_fridge.c
        project/src/include/device/interaction/device_fridge.h
        project/src/include/cli/command/command_add.h project/src/include/cli/command/command_del.h project/src/include/cli/command/command_link.h project/src/include/cli/command/command_switch.h project/src/include/cli/command/command_info.h project/src/src/cli/command/command_add.c project/src/src/cli/command/command_del.c project/src/src/cli/command/command_info.c project/src/src/cli/command/command_link.c project/src/src/cli/command/command_switch.c project/src/include/collection/collection_list.h project/src/src/collection/collection_list.c project/src/include/cli/command/command_device.h project/src/src/cli/command/command_device.c project/src/include/util/util_os.h project/src/src/util/util_os.c project/src/include/device/device_communication.h project/src/src/device/device_communication.c project/src/include/device/device_child.h project/src/src/device/device_child.c project/src/include/util/util_converter.h project/src/src/util/util_converter.c project/src/include/device/interaction/device_window.h project/src/src/device/interaction/device_window.c project/src/src/device/domus_manual.c project/src/include/device/domus_manual.h project/src/src/cli/command/command_connect.c project/src/include/cli/command/command_connect.h project/src/src/device/control_libs.c project/src/include/device/control_libs.h project/src/src/cli/command/command_test.c project/src/include/cli/command/command_test.h project/src/src/cli/command/command_connect_manual.c project/src/include/cli/command/command_connect_manual.h project/src/include/cli/command/command_hierarchy.h project/src/src/cli/command/command_hierarchy.c
        project/src/include/cli/command/command_add.h project/src/include/cli/command/command_del.h project/src/include/cli/command/command_link.h project/src/include/cli/command/command_switch.h project/src/include/cli/command/command_info.h project/src/src/cli/command/command_add.c project/src/src/cli/command/command_del.c project/src/src/cli/command/command_info.c project/src/src/cli/command/command_link.c project/src/src/cli/command/command_switch.c project/src/include/collection/collection_list.h project/src/src/collection/collection_list.c project/src/include/cli/command/command_device.h project/src/src/cli/command/command_device.c project/src/include/util/util_os.h project/src/src/util/util_os.c project/src/include/device/device_communication.h project/src/src/device/device_communication.c project/src/include/device/device_child.h project/src/src/device/device_child.c project/src/include/util/util_converter.h project/src/src/util/util_converter.c project/src/include/device/interaction/device_window.h project/src/src/device/interaction/device_window.c project/src/src/device/domus_manual.c project/src/include/device/domus_manual.h project/src/src/cli/command/command_connect.c project/src/include/cli/command/command_connect.h project/src/src/device/control_libs.c project/src/include/device/control_libs.h project/src/src/cli/command/command_test.c project/src/include/cli/command/command_test.h project/src/src/cli/command/command_connect_manual.c project/src/include/cli/command/command_connect_manual.h project/src/src/cli/command/command_switch_manual.c project/src/include/cli/command/command_switch_manual.h)
add_executable(domus ${SOURCE_FILES})