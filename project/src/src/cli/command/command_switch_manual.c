#include "cli/command/command_switch_manual.h"
#include "cli/command/command.h"
#include "util/util_converter.h"
#include "util/util_printer.h"
#include "cli/cli.h"
#include "device/control_libs.h"

/**
 * Manual switch on devices
 * @param args Arguments
 * @return CLI status code
 */
static int switch_manual(char **args) {
    ConverterResult result;

    if(args[1] == NULL || (result = converter_string_to_long(args[1])).error || args[2] == NULL || args[3] == NULL){
        println_color(COLOR_RED, "\tPlease type a valid pattern");
        println_color(COLOR_YELLOW, "\t\tswitch <id> <label> <pos>");
        return CLI_CONTINUE;
    }
    manual_control_set_device(result.data.Long, args[2], args[3]);
    return CLI_CONTINUE;
}

Command *command_switch_manual(void) {
    return new_command(
            "switch",
            "Manually switch the device with <id> the feature <label> into <pos>",
            "switch <id> <label> <pos>",
            switch_manual);
}