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

    if(args[1] == NULL || (result = converter_string_to_long(args[1])).error){
        println_color(COLOR_RED, "\tPlease type a valid Device ID");
        return CLI_CONTINUE;
    }
    println_color(COLOR_GREEN, "\tTest function\n");
    device_id = manual_control_get_device_pid(result.data.Long);

    if(device_id == 0){
        println_color(COLOR_RED, "\tDevice not found or not started yet");
        return CLI_CONTINUE;
    }

    return CLI_CONTINUE;
}

Command *command_test(void) {
    return new_command(
            "test",
            "Test command",
            "test",
            test);
}