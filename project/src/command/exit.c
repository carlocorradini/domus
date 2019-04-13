
#include "command/exit.h"
#include "cli.h"
#include "printer.h"

/**
 * Close Domus
 * @param args Arguments
 * @return CLI status code
 */
static int __exit(char **args) {
    println_color(COLOR_GREEN, "\tThanks for using Domus");
    return CLI_TERMINATE;
}

Command *command_exit(void) {
    return new_command(
            "exit",
            "Close Domus",
            "exit",
            __exit);
}