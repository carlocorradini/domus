
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
    ConverterResult result;

    if (args[1] == NULL) {
        println("\tPlease add a device id");
    } else {
        result = converter_char_to_long(args[1]);

        if (result.error) {
            println("\tConversion Error: %s", result.error_message);
        } else if (result.data.Long == 0) {
            println("\tCannot delete the Controller");
        } else if (controller_valid_id(result.data.Long) == -1) {
            println("\tCannot find a Device with id %ld", result.data.Long);
        } else {
            controller_del(result.data.Long);
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