
#include "printer.h"
#include "cli/cli.h"
#include "cli/command/command_add.h"
#include "device/device.h"

/**
 * Add a device to the system and show its features
 * @param args Arguments
 * @return CLI status code
 */
static int _add(char **args) {
    if (args[1] == NULL) {
        println("\tPlease choose a device");
    } else if (!device_is_supported(args[1])) {
        println("\tDevice <%s> is not supported", args[1]);
    } else {
        println("\tDevice <%s> is supported", args[1]);
    }

    return CLI_CONTINUE;
}

Command *command_add(void) {
    return new_command(
            "add",
            "Add a <device> to the system and show its features",
            "add <device>",
            _add);
}