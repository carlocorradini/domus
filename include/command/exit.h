#ifndef _EXIT_H
#define _EXIT_H

#include "command/command.h"

static int _exit(char **args) {
    printf("MESSAGGIO DI EXIT\n");
    return CLI_TERMINATE;
}

Command *command_exit(void) {
    return new_command(
            "exit",
            "Close Domus",
            "exit",
            _exit);
}

#endif //_EXIT_H
