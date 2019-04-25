
#include <stdio.h>
#include <errno.h>
#include "cli/cli.h"
#include "cli/command/command_del.h"
#include "util/util_printer.h"
#include "util/util_converter.h"
#include "device/control/device_controller.h"

/**
 * Delete the device with id from the system
 *  If it's a control device, deletion is done recursively
 * @param args Arguments
 * @return CLI status code
 */
static int _del(char **args) {
    if (args[1] == NULL) {
        println("\tPlease add a device id");
    } else {
        size_t id = converter_char_to_long(args[1]);
        if (controller_valid_id(id) == -1) {
            println("\tCannot find a Device with id %ld", id);
        } else {
            controller_del(id);
        }
    }

    return CLI_CONTINUE;
}

Command *command_del(void) {
    return new_command(
            "del",
            "Delete the device with <id> from the system. If it's a control device, deletion is done recursively",
            "del <id>",
            _del);
}