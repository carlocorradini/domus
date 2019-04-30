
#include <stdio.h>
#include <string.h>
#include "cli/cli.h"
#include "cli/command/command_link.h"
#include "util/util_converter.h"
#include "util/util_printer.h"
#include "device/control/device_controller.h"

/**
 * Connect two devices each other
 *  One must be a control device
 * @param args Arguments
 * @return CLI status code
 */
static int _link(char **args) {
    ConverterResult device_id;
    ConverterResult control_device_id;

    if (args[1] == NULL) {
        println("\tPlease add a device id");
    } else if (args[2] == NULL) {
        println("\tPlease add 'to' label");
    } else if (args[3] == NULL) {
        println("\tPlease add a control device id");
    } else if (!controller_has_devices()) {
        println("\tNo Devices");
    } else {
        device_id = converter_string_to_long(args[1]);
        control_device_id = converter_string_to_long(args[3]);

        if (device_id.error) {
            println("\tDevice Conversion Error: %s", device_id.error_message);
        } else if (control_device_id.error) {
            println("\tControl Device Conversion Error: %s", control_device_id.error_message);
        } else if (strcmp(args[2], "to") != 0) {
            println("\t%s is invalid, add 'to' label", args[2]);
        } else if (control_device_id.error) {
            println("\tControl Device Conversion Error: %s", device_id.error_message);
        } else {
            controller_link(device_id.data.Long, control_device_id.data.Long);
        }
    }

    return CLI_CONTINUE;
}

Command *command_link(void) {
    return new_command(
            "link",
            "Connect two devices each other. One must be a control device",
            "link <id> to <id>",
            _link);
}