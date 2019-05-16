
#include "domus.h"
#include "util/util_printer.h"
#include "cli/cli.h"
#include "cli/command/command_add.h"
#include "device/device.h"

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
    } else if ((device_descriptor = device_is_supported_by_name(args[1])) == NULL ||
               device_descriptor->id == DEVICE_TYPE_DOMUS) {
        println("\tDevice %s is not supported", args[1]);
    } else if (device_descriptor->id == CONTROLLER_ID) {
        println("\tCannot add another Controller, only one is allowed");
    } else if ((id = domus_fork_device(device_descriptor, args[2])) == -1) {
        println_color(COLOR_RED, "\tSomething goes wrong");
    } else {
        println_color(COLOR_GREEN, "\t%s added with id %ld", device_descriptor->name, id);
        println("");
        domus_info_by_id(id);
    }

    return CLI_CONTINUE;
}

Command *command_add(void) {
    return new_command(
            "add",
            "Add a <device> to the system and show its features. Add [name] to define a custom name for the <device>",
            "add <device> [name]",
            _add);
}