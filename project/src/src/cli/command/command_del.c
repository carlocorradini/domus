
#include <string.h>
#include "domus.h"
#include "cli/cli.h"
#include "cli/command/command_del.h"
#include "util/util_printer.h"
#include "util/util_converter.h"

/**
 * Delete the device with id from the system
 *  If it's a control device, deletion is done recursively
 * @param args Arguments
 * @return CLI status code
 */
static int _del(char **args) {
    ConverterResult result;

    if (domus_system_is_active()) {
        if (args[1] == NULL) {
            println("\tPlease add a device id");
        } else if (!domus_has_devices()) {
            println("\tNo Devices");
        } else if (strcmp(args[1], COMMAND_DEL_ALL) == 0) {
            if (!domus_del_all()) {
                println("\tNo Device to Delete");
            }
        } else {
            result = converter_string_to_long(args[1]);

            if (result.error) {
                println("\tConversion Error: %s", result.error_message);
            } else if (result.data.Long == CONTROLLER_ID) {
                println("\tCannot delete the Controller");
            } else if (!domus_del_by_id(result.data.Long)) {
                println("\tCannot find a Device with id %ld", result.data.Long);
            }
        }
    }

    return CLI_CONTINUE;
}

Command *command_del(void) {
    return new_command(
            "del",
            "Delete the device with <id>. If [--all] delete all devices. If it's a control device, deletion is done recursively",
            "del <id> [--all]",
            _del);
}