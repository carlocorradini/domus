
#include <stdio.h>
#include <string.h>
#include "cli/cli.h"
#include "cli/command/command_info.h"
#include "util/util_printer.h"
#include "util/util_converter.h"
#include "device/control/device_controller.h"

/**
 * Print the header of the Device Information
 */
static void command_info_print_header(void);

/**
 * Show device info with id
 * @param args Arguments
 * @return CLI status code
 */
static int _info(char **args) {
    ConverterResult result;

    if (args[1] == NULL) {
        println("\tPlease add a device id");
    } else if (strncmp(args[1], COMMAND_INFO_ALL, strlen(COMMAND_INFO_ALL)) == 0) {
        if (!controller_has_devices()) {
            println("\tNo Devices");
        } else {
            command_info_print_header();
            controller_info_all();
        }
    } else {
        result = converter_char_to_long(args[1]);

        if (result.error) {
            println("\tConversion Error: %s", result.error_message);
        } else if (result.data.Long == 0) {
            println("\tCannot show controller info");
        } else if (controller_valid_id(result.data.Long) == -1) {
            println("\tCannot find a Device with id %ld", result.data.Long);
        } else {
            command_info_print_header();
            controller_info_by_id(result.data.Long);
        }
    }


    return CLI_CONTINUE;
}

static void command_info_print_header(void) {
    println_color(COLOR_YELLOW, "\t%-*s     %-*s     %-*s",
                  sizeof(size_t) + 1, "ID",
                  DEVICE_NAME_LENGTH, "NAME",
                  DEVICE_DESCRIPTION_LENGTH, "DESCRIPTION");
}

Command *command_info(void) {
    return new_command(
            "info",
            "Show device info with <id>. Show all devices info with [--all]",
            "info <id> [--all]",
            _info);
}