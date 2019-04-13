
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command/command.h"
#include "cli.h"
#include "printer.h"

/* Commands */
#include "command/help.h"
#include "command/exit.h"

/* END Commands */

List *commands = NULL;

void commands_init(void) {
    if (commands != NULL) return;
    commands = list_create(NULL);

    list_push(commands, command_help());
    list_push(commands, command_exit());
}

void commands_free(void) {
    list_free(commands);
}

Command *new_command(char name[], char description[], char syntax[], int (*execute)(char **)) {
    Command *command = (Command *) malloc(sizeof(Command));
    if (command == NULL) {
        perror("Command Memory Allocation");
        exit(EXIT_FAILURE);
    }

    strncpy(command->name, name, COMMAND_NAME_LENGTH);
    strncpy(command->description, description, COMMAND_DESCRIPTION_LENGTH);
    strncpy(command->syntax, syntax, COMMAND_SYNTAX_LENGTH);
    command->execute = execute;
    return command;
}

/* \todo Implement with HashMap O(1) instead of O(n) */
int command_execute(char **args) {
    Command *command;
    if (args[0] == NULL) {
        /* No Command passed, CONTINUE */
        return CLI_CONTINUE;
    }

    list_for_each(item, commands)
    {
        command = (Command *) item->data;
        if (strcmp(args[0], command->name) == 0) {
            /* Command Found */
            if (args[1] && strcmp(args[1], CLI_QUESTION) == 0) {
                /* Command Question */
                command_information(command);
                return CLI_CONTINUE;
            }
            /* Execute Command */
            return command->execute(args);
        }
    }
    return -1;
}

void command_information(const Command *command) {
    print_color(COLOR_YELLOW, "\t%-*s", COMMAND_SYNTAX_LENGTH, command->syntax);
    println("%s", command->description);
}