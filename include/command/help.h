#ifndef _HELP_H
#define _HELP_H

#include "command/command.h"

static int _help(char **args) {
    printf("MESSAGGIO DI HELP\n");
    return CLI_CONTINUE;
}

Command *command_help(void) {
    return new_command(
            "help",
            "Display help information about Domus",
            "help",
            _help);
}

#endif //_HELP_H
