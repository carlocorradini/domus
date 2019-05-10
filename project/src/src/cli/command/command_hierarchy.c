
#include "cli/command/command_hierarchy.h"
#include "domus.h"
#include "cli/cli.h"

/**
 * Display the current Device hierarchy in the system
 * @param args Arguments
 * @return CLI status code
 */
static int _hierarchy(char **args) {
    domus_hierarchy();

    return CLI_CONTINUE;
}

Command *command_hierarchy(void) {
    return new_command(
            "hierarchy",
            "Display the current devices hierarchy in the system",
            "hierarchy",
            _hierarchy);
}