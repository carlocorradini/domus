
#include <stdio.h>
#include <string.h>
#include "cli/cli.h"
#include "cli/command/command_info.h"
#include "util/util_printer.h"
#include "util/util_converter.h"
#include "device/control/device_controller.h"

/**
 * Show device info with id
 * @param args Arguments
 * @return CLI status code
 */
static int _info(char **args) {
    ConverterResult result;

    if (args[1] == NULL) {
        println("\tPlease add a device id");
    } else if (!controller_has_devices()) {
        println("\tNo Devices");
    } else if (strcmp(args[1], COMMAND_INFO_ALL) == 0) {
        controller_info_all();
    } else {
        result = converter_string_to_long(args[1]);

        if (result.error) {
            println("\tConversion Error: %s", result.error_message);
        } else if (result.data.Long == DEVICE_CONTROLLER_ID) {
            println("\tCannot show controller info");
        } else if (!controller_info_by_id(result.data.Long)) {
            println("\tCannot find a Device with id %ld", result.data.Long);
        }
    }


    return CLI_CONTINUE;
}

Command *command_info(void) {
    return new_command(
            "info",
            "Show device info with <id>. Show all devices info with [--all]",
            "info <id> [--all]",
            _info);
}