
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
    ConverterResult device_id;

    if (domus_system_is_active()) {
        if (args[1] == NULL) {
            println("\tPlease enter a Device id");
        } else if ((device_id = converter_string_to_long(args[1])).error) {
            println("\tConversion Error: %s", device_id.error_message);
        } else if (args[2] == NULL || args[3] == NULL) {
            println_color(COLOR_RED, "\tPlease type a valid pattern:");
            println_color(COLOR_YELLOW, "\t\tswitch <id> <label> <pos>");
        } else {
            domus_switch(device_id.data.Long, args[2], args[3]);
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