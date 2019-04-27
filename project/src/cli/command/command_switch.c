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
        println("\tPlease enter a device id");
        return CLI_CONTINUE;
    } else {
        result = converter_string_to_long(args[1]);
        if (result.error) {
            println("\tConversion Error: %s", result.error_message);
        } else if (result.data.Long == 0) {
            println("\tCannot switch the Controller");
        } else if (!controller_valid_id(result.data.Long)) {
            println("\tCannot find a Device with id %ld", result.data.Long);
        } else {
            switch (controller_switch((size_t) result.data.Long, args[2], args[3])) {
                case 0: {
                    print_color(COLOR_GREEN, "\tSwitched ");
                    print("'%s'", args[2]);
                    print_color(COLOR_GREEN, " to ");
                    println("'%s'", args[3]);

                    break;
                }
                case 1: {
                    println_color(COLOR_RED,
                                  "\tError while setting switch '%s': <label> doesn't exist",
                                  args[2], args[3]);
                    break;
                }
                case 2: {
                    println_color(COLOR_RED,
                                  "\tError while setting switch '%s': <pos> doesn't exist",
                                  args[2], args[3]);
                    break;
                }
                default: {
                    println_color(COLOR_RED, "\tError while setting <label> '%s' to <pos> '%s': Unknown error",
                                  args[2],
                                  args[3]);
                    break;
                }
            }
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