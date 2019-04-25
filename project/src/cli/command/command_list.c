
#include <stdio.h>
#include "cli/cli.h"
#include "cli/command/command_list.h"
#include "device/control/device_controller.h"

/**
 * Display all available devices and their features
 * @param args Arguments
 * @return CLI status code
 */
static int _list(char **args) {
    controller_list();

    return CLI_CONTINUE;
}

Command *command_list(void) {
    return new_command(
            "list",
            "Display all available devices and their features",
            "list",
            _list);
}