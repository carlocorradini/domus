#include <stdio.h>
#include "cli/cli.h"
#include "cli/command/command_switch.h"
#include "util/util_converter.h"
#include "util/util_printer.h"
#include "device/control/device_controller.h"

/**
 * Switch the device with id the feature label into the position pos
 * @param args Arguments
 * @return CLI status code
 */
static int _switch(char **args) {
    ConverterResult result;

    if (args[1] == NULL) {
        println("\tPlease enter a valid device id");
        return CLI_CONTINUE;
    }
    result = converter_char_to_long(args[1]);
    if (result.error) {
        println("\tPlease enter a valid device id");
        return CLI_CONTINUE;
    }
    if (controller_valid_id((size_t) result.data.Long) == -1) {
        println("\tPlease enter a valid device id");
        return CLI_CONTINUE;
    }
    if (set_device_switch((size_t) result.data.Long, args[2], args[3])) {
    } else {
        println("\tError while setting switch %s", args[2]);
    }

    return CLI_CONTINUE;
}

Command *command_switch(void) {
    return new_command(
            "switch",
            "Switch the device with <id> the feature <label> into <pos>",
            "switch <id> <label> <pos>",
            _switch);
}