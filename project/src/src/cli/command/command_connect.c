
#include "cli/command/command.h"
#include "cli/command/command_connect.h"
#include "cli/cli.h"
#include "util/util_printer.h"
#include "domus.h"


static int _connect(char **args) {
    println_color(COLOR_YELLOW, "\tDomus PID is %ld", domus_getpid(DOMUS_ID));
    println_color(COLOR_YELLOW, "\tNow you can launch domus_manual application and type:");
    println_color(COLOR_YELLOW, "\t\tconnect %ld", domus_getpid(DOMUS_ID));
    return CLI_CONTINUE;
}

Command *command_connect(void) {
    return new_command(
            "connect",
            "Get unique Domus PID for connecting Domus Manual control interface to Domus",
            "connect",
            _connect);
}