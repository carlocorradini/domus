#include "cli/command/command_connect_manual.h"


static int _manual_connect(char **args) {
    __pid_t domus_pid;
    ConverterResult result;

    if(args[1] == NULL || (result = converter_string_to_long(args[1])).error){
        println_color(COLOR_RED, "\tPlease type a valid Domus PID");
        return CLI_CONTINUE;
    }

    domus_pid = result.data.Long;
    if(manual_control_check_domus(domus_pid)) {
        println_color(COLOR_GREEN, "\tSuccessfully established connection with Domus");
    } else{
        println_color(COLOR_RED, "\tError while establishing connection with Domus");
    }
    return CLI_CONTINUE;
}

Command *command_connect_manual(void) {
    return new_command(
            "connect",
            "Connect to Domus",
            "connect",
            _manual_connect);
}