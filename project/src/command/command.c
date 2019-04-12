
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data_structure/list.h"
#include "command/command.h"
// Commands
#include "command/help.h"
#include "command/exit.h"
// END Commands

List *commands = NULL;

void commands_init(void) {
    if (commands) return;
    commands = list_create(NULL);

    list_push(commands, command_help());
    list_push(commands, command_exit());
}

/*
void commands_free(void) {
    if (!commands) return;

    Command *command = NULL;
    list_for_each(item, commands) {
        command = (Command *) item->data;
        free_command(command);
    }
    list_free(commands);
}
*/

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
/*
void free_command(Command *command) {
    if (!command) return;
    free(command->name);
    free(command->description);
    free(command->syntax);
    command->execute = NULL;
    free(command);
}
*/
int command_execute(char **args) {
    if (args[0] == NULL) {
        // No Command passed, CONTINUE
        return CLI_CONTINUE;
    }

    Command *command = NULL;
    list_for_each(item, commands) { // nice to implement --> hash map or something O(1) instead of cicling (O(n))
        command = (Command *) item->data;
        if (strcmp(args[0], command->name) == 0) {
            // Command Found
            if (args[1] && strcmp(args[1], CLI_QUESTION) == 0) {
                // Command Question
                command_information(command);
                return CLI_CONTINUE;
            }
            // Execute Command
            return command->execute(args);
        }
    }
    return -1;
}

static void command_information(const Command *command) {
    print("\t%-*s", COMMAND_SYNTAX_LENGTH, command->syntax);
    println("%s", command->description);
}