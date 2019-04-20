
#include "util/util_printer.h"
#include "cli/cli.h"
#include "cli/command/command_add.h"
#include "device/device.h"
#include "device/control/device_controller.h"

/**
 * Add a device to the system and show its features
 * @param args Arguments
 * @return CLI status code
 */
static int _add(char **args) {
    const DeviceDescriptor *device_descriptor;
    if (args[1] == NULL) {
        println("\tPlease choose a device");
    } else if ((device_descriptor = device_is_supported(args[1])) == NULL) {
        println("\tDevice %s is not supported", args[1]);
    } else {
        if (controller_fork_device(device_descriptor)) {
            println_color(COLOR_GREEN, "\tDevice %s has been added", device_descriptor->name);
            println_color(COLOR_GREEN, "\t\t# %u directly connected device/s", controller_connected_directly());
            println_color(COLOR_GREEN, "\t\t# %u total connected device/s", (controller_connected_total()));
        } else {
            println_color(COLOR_RED, "\tSomething goes wrong");
        }
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