
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cli/command/command.h"
#include "cli/cli.h"
#include "printer.h"

/* Supported Commands */
#include "cli/command/command_add.h"
#include "cli/command/command_del.h"
#include "cli/command/command_exit.h"
#include "cli/command/command_help.h"
#include "cli/command/command_info.h"
#include "cli/command/command_link.h"
#include "cli/command/command_list.h"
#include "cli/command/command_switch.h"

/* END Supported Commands */

List *commands = NULL;

void commands_init(void) {
    if (commands != NULL) return;
    commands = list_create(NULL);

    list_push(commands, command_add());
    list_push(commands, command_del());
    list_push(commands, command_exit());
    list_push(commands, command_help());
    list_push(commands, command_info());
    list_push(commands, command_link());
    list_push(commands, command_list());
    list_push(commands, command_switch());
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

    list_for_each(item, commands) {
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