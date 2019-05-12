#include "cli/command/command_test.h"
#include "cli/cli.h"
#include "util/util_printer.h"
#include "device/control_libs.h"

/**
 * Close Domus
 * @param args Arguments
 * @return CLI status code
 */
static int test(char **args) {
    ConverterResult result;
    __pid_t device_id;

    if(args[1] == NULL || (result = converter_string_to_long(args[1])).error || args[2] == NULL || args[3] == NULL){
        println_color(COLOR_RED, "\tPlease type a valid Device ID");
        return CLI_CONTINUE;
    }
    println_color(COLOR_GREEN, "\tTest function\n");
    println_color(COLOR_GREEN, "\tManual set device %lu : %d\n", result.data.Long, manual_control_set_device(result.data.Long, args[2], args[3]));

    return CLI_CONTINUE;
}

Command *command_test(void) {
    return new_command(
            "test",
            "Test command",
            "test",
            test);
}