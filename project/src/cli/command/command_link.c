
#include <stdio.h>
#include "cli/cli.h"
#include "cli/command/command_link.h"

/**
 * Connect two devices each other
 *  One must be a control device
 * @param args Arguments
 * @return CLI status code
 */
static int _link(char **args) {
    printf("\tNOT SUPPORTED YET\n");

    return CLI_CONTINUE;
}

Command *command_link(void) {
    return new_command(
            "link",
            "Connect two devices each other. One must be a control device",
            "link <id> to <id>",
            _link);
}