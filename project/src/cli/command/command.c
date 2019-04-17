
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cli/command/command.h"
#include "cli/cli.h"
#include "printer.h"

/* Supported Commands */
#include "cli/command/command_add.h"
#include "cli/command/command_clear.h"
#include "cli/command/command_del.h"
#include "cli/command/command_exit.h"
#include "cli/command/command_help.h"
#include "cli/command/command_info.h"
#include "cli/command/command_link.h"
#include "cli/command/command_list.h"
#include "cli/command/command_switch.h"

/* END Supported Commands */

/**
 * List of Supported Commands
 */
static LinkedList *commands = NULL;

void commands_init(void) {
    if (commands != NULL) return;
    commands = new_linked_list(NULL, NULL);

    linked_list_add_last(commands, command_add());
    linked_list_add_last(commands, command_clear());
    linked_list_add_last(commands, command_del());
    linked_list_add_last(commands, command_exit());
    linked_list_add_last(commands, command_help());
    linked_list_add_last(commands, command_info());
    linked_list_add_last(commands, command_link());
    linked_list_add_last(commands, command_list());
    linked_list_add_last(commands, command_switch());
}

void commands_free(void) {
    free_linked_list(commands);
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

LinkedList *command_get_commands() {
    return commands;
}

/* \todo Implement with HashMap O(1) instead of O(n) */
int command_execute(char **args) {
    Command *data;
    if (args[0] == NULL) {
        /* No Command passed, CONTINUE */
        return CLI_CONTINUE;
    }

    list_for_each(data, commands) {
        if (strcmp(args[0], data->name) == 0) {
            /* Command Found */
            if (args[1] && strcmp(args[1], CLI_QUESTION) == 0) {
                /* Command Question */
                command_information(data);
                return CLI_CONTINUE;
            }
            /* Execute Command */
            return data->execute(args);
        }
    }

    return -1;
}

void command_information(const Command *command) {
    print_color(COLOR_YELLOW, "\t%-*s", COMMAND_SYNTAX_LENGTH, command->syntax);
    println("%s", command->description);
}