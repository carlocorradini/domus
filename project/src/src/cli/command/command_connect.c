#include "cli/command/command.h"
#include "cli/command/command_connect.h"
#include "cli/cli.h"
#include "util/util_printer.h"
#include "domus.h"
#include <unistd.h>

static int _connect(char **args) {
    println_color(COLOR_YELLOW, "\tDomus PID is %ld", domus_getpid(1));
    println_color(COLOR_YELLOW, "\tNow you can launch manual_control application and type: ");
    println_color(COLOR_YELLOW, "\t\tconnect %ld", domus_getpid(1));
    return CLI_CONTINUE;
}

Command *command_connect(void) {
    return new_command(
            "connect",
            "Connect manual control interface to Domus",
            "connect",
            _connect);
}