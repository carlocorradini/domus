
#include "cli/cli.h"
#include "cli/command/command_help.h"
#include "collection/collection_list.h"

/**
 * Display help information about Domus
 * @param args Arguments
 * @return CLI status code
 */
static int _help(char **args) {
    command_print_all();

    return CLI_CONTINUE;
}

Command *command_help(void) {
    return new_command(
            "help",
            "Display help information about Domus",
            "help",
            _help);
}