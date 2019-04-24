
#include <stdio.h>
#include "cli/cli.h"
#include "cli/command/command_switch.h"

/**
 * Switch the device with id the feature label into the position pos
 * @param args Arguments
 * @return CLI status code
 */
static int _switch(char **args) {
    printf("\tNOT SUPPORTED YET\n");

    return CLI_CONTINUE;
}

Command *command_switch(void) {
    return new_command(
            "switch",
            "Switch the device with <id> the feature <label> into <pos>",
            "switch <id> <label> <pos>",
            _switch);
}