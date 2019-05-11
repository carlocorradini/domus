#include "cli/command/command_test.h"
#include "cli/cli.h"
#include "util/util_printer.h"

/**
 * Close Domus
 * @param args Arguments
 * @return CLI status code
 */
static int test(char **args) {
    println_color(COLOR_GREEN, "\tTest function");
    return CLI_CONTINUE;
}

Command *command_test(void) {
    return new_command(
            "test",
            "Test command",
            "test",
            test);
}