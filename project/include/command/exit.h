#ifndef _EXIT_H
#define _EXIT_H

#include "command/command.h"

static int __exit(char **args) {
    println("\tThanks for using Domus");
    return CLI_TERMINATE;
}

Command *command_exit(void) {
    return new_command(
            "exit",
            "Close Domus",
            "exit",
            __exit);
}

#endif //_EXIT_H
