
#include "domus.h"
#include <stdio.h>
#include "cli/cli.h"
#include "cli/command/command_switch.h"
#include "util/util_converter.h"
#include "util/util_printer.h"

/**
 * Switch the device with id the feature label into the position pos
 * @param args Arguments
 * @return CLI status code
 */
static int _switch(char **args) {
    ConverterResult result;

    if (args[1] == NULL) {
        println("\tPlease enter a device id");
        return CLI_CONTINUE;
    } else {
        result = converter_string_to_long(args[1]);
        if (result.error) {
            println("\tConversion Error: %s", result.error_message);
        } else {
            domus_switch(result.data.Long, args[2], args[3]);
        }
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