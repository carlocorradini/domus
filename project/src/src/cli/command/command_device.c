
#include "cli/cli.h"
#include "cli/command/command_help.h"
#include "device/device.h"

/**
 * Display supported devices in Domus
 * @param args Arguments
 * @return CLI status code
 */
static int _device(char **args) {
    device_print_all();

    return CLI_CONTINUE;
}

Command *command_device(void) {
    return new_command(
            "device",
            "Display all supported devices and their description",
            "device",
            _device);
}