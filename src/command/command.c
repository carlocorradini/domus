
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data_structure/list.h"
#include "command/command.h"
// Commands
#include "command/help.h"
#include "command/exit.h"
// END Commands

List *command_get_all(void) {
    List *commands = list_create(NULL);

    list_push(commands, command_help());
    list_push(commands, command_exit());

    return commands;
}

Command *new_command(char name[], char description[], char syntax[], int (*execute)(char **)) {
    Command *command = (Command *) malloc(sizeof(Command));
    if (!command) {
        perror("Command Memory Allocation");
        exit(EXIT_FAILURE);
    }

    strncpy(command->name, name, COMMAND_NAME_LENGTH);
    strncpy(command->description, description, COMMAND_DESCRIPTION_LENGTH);
    strncpy(command->syntax, syntax, COMMAND_SYNTAX_LENGTH);
    command->execute = execute;
    return command;
}

void free_command(Command *command) {
    free(command->name);
    free(command->description);
    free(command->syntax);
    command->execute = NULL;
    free(command);
}