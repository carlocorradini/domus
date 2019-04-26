
#include <stdio.h>
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
    } else {
        result = converter_char_to_long(args[1]);

        if (result.error) {
            println("\tConversion Error: %s", result.error_message);
        } else if (result.data.Long == 0) {
            println("\tCannot show controller info");
        } else if (controller_valid_id(result.data.Long) == -1) {
            println("\tCannot find a Device with id %ld", result.data.Long);
        } else {
            getInfo(result.data.Long);
        }
    }


    return CLI_CONTINUE;
}

Command *command_info(void) {
    return new_command(
            "info",
            "Show device info with <id>",
            "info <id>",
            _info);
}