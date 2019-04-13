
#include "command/command_list.h"
#include "include/cli/cli.h"

/**
 * Display all available devices and their features
 * @param args Arguments
 * @return CLI status code
 */
static int _list(char **args) {
    printf("\tNOT SUPPORTED YET\n");

    return CLI_CONTINUE;
}

Command *command_list(void) {
    return new_command(
            "list",
            "Display all available devices and their features",
            "list",
            _list);
}