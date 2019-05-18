
#include "cli/command/command_connect_manual.h"

static int _manual_connect(char **args) {
    ConverterResult domus_pid;

    if (args[1] == NULL) {
        println_color(COLOR_RED, "\tPlease type a valid Domus PID");
    } else if ((domus_pid = converter_string_to_long(args[1])).error) {
        println_color(COLOR_RED, "\tConversion error: %s", domus_pid.error_message);
    } else if (!manual_control_check_domus(domus_pid.data.Long)) {
        println_color(COLOR_RED, "\tError while establishing connection with Domus");
    } else {
        println_color(COLOR_GREEN, "\tSuccessfully established connection with Domus");
    }

    return CLI_CONTINUE;
}

Command *command_connect_manual(void) {
    return new_command(
            "connect",
            "Connect to Domus using unique <PID>",
            "connect <PID>",
            _manual_connect);
}