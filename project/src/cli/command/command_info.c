
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
    size_t id;
    if (args[1] == NULL) {
        println("\tPlease add a device id");
    } else {
        id = (size_t) converter_char_to_long(args[1]);

        if(controller_valid_id(id)==-1){
            println("\tPlease enter a valid device id");
        }
        else{
            getInfo(id);
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