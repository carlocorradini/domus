
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
    size_t id;

    if (args[1] == NULL) {
        println("\tPlease choose a device");
    } else if ((device_descriptor = device_is_supported(args[1])) == NULL) {
        println("\tDevice %s is not supported", args[1]);
    } else if ((id = controller_fork_device(device_descriptor)) == -1) {
        println_color(COLOR_RED, "\tSomething goes wrong");
    } else {
        println_color(COLOR_GREEN, "\t%s added with id %ld", device_descriptor->name, id);
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