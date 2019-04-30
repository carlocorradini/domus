
#include <stdlib.h>
#include <stdbool.h>
#include "cli/cli.h"
#include "util/util_os.h"
#include "cli/command/command_clear.h"

/**
 * Clear the CLI interface
 * @param args Arguments
 * @return CLI status code
 */
static int _clear(char **args) {
    (os_windows()) ? system("CLS") : system("clear");
    return CLI_CONTINUE;
}

Command *command_clear(void) {
    return new_command(
            "clear",
            "Clear the CLI interface",
            "clear",
            _clear);
}