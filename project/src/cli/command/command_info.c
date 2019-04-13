
#include "cli/cli.h"
#include "cli/command/command_info.h"

/**
 * Show device info with id
 * @param args Arguments
 * @return CLI status code
 */
static int _info(char **args) {
    printf("\tNOT SUPPORTED YET\n");

    return CLI_CONTINUE;
}

Command *command_info(void) {
    return new_command(
            "info",
            "Show device info with <id>",
            "info <id>",
            _info);
}