
#include <stdio.h>
#include "cli/cli.h"
#include "cli/command/command_list.h"
#include "util/util_printer.h"
#include "device/control/device_controller.h"

/**
 * Display all available devices and their features
 * @param args Arguments
 * @return CLI status code
 */
static int _list(char **args) {
    if (!controller_has_devices()) {
        println("\tNo Devices");
    } else {
        controller_list();
    }

    return CLI_CONTINUE;
}

Command *command_list(void) {
    return new_command(
            "list",
            "Display all available devices and their features",
            "list",
            _list);
}