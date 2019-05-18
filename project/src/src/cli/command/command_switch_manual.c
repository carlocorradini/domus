
#include "cli/command/command_switch_manual.h"
#include "cli/command/command.h"
#include "util/util_converter.h"
#include "util/util_printer.h"
#include "cli/cli.h"
#include "device/manual_libs.h"

/**
 * Manual switch on devices
 * @param args Arguments
 * @return CLI status code
 */
static int switch_manual(char **args) {
    ConverterResult device_id;

    if (args[1] == NULL) {
        println("\tPlease enter a Device id");
    } else if ((device_id = converter_string_to_long(args[1])).error) {
        println("\tConversion Error: %s", device_id.error_message);
    } else if (args[2] == NULL || args[3] == NULL) {
        println_color(COLOR_RED, "\tPlease type a valid pattern:");
        println_color(COLOR_YELLOW, "\t\tswitch <id> <label> <pos>");
    } else {
        manual_control_set_device(device_id.data.Long, args[2], args[3]);
    }

    return CLI_CONTINUE;
}

Command *command_switch_manual(void) {
    return new_command(
            "switch",
            "Manually switch the device with <id> the feature <label> into <pos>",
            "switch <id> <label> <pos>",
            switch_manual);
}