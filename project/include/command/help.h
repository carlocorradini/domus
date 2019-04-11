#ifndef _HELP_H
#define _HELP_H

#include "command/command.h"
#include "printer.h"

static int _help(char **args) {
    if (!commands) return CLI_CONTINUE;

    Command *command = NULL;
    list_for_each(item, commands) {
        command = (Command *) item->data;
        command_information(command);
    }

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
