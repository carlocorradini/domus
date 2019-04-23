
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "collection/collection_list.h"
#include "collection/collection_trie.h"
#include "cli/command/command.h"
#include "cli/cli.h"
#include "util/util_printer.h"

/* Supported Commands */
#include "cli/command/command_add.h"
#include "cli/command/command_clear.h"
#include "cli/command/command_del.h"
#include "cli/command/command_device.h"
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
static List *commands = NULL;
/**
 * Trie of Supported Commands
 */
static Trie *autocomplete = NULL;

void command_init(void) {
    if (commands != NULL || autocomplete != NULL) return;
    commands = new_list(NULL, NULL);
    autocomplete = new_trie(NULL, NULL);

    list_add_last(commands, command_add());
    autocomplete = trie_insert(autocomplete, command_add()->name, 1);
    list_add_last(commands, command_clear());
    autocomplete = trie_insert(autocomplete, command_clear()->name, 1);
    list_add_last(commands, command_del());
    autocomplete = trie_insert(autocomplete, command_del()->name, 1);
    list_add_last(commands, command_device());
    autocomplete = trie_insert(autocomplete, command_device()->name, 1);
    list_add_last(commands, command_exit());
    autocomplete = trie_insert(autocomplete, command_exit()->name, 1);
    list_add_last(commands, command_help());
    autocomplete = trie_insert(autocomplete, command_help()->name, 1);
    list_add_last(commands, command_info());
    autocomplete = trie_insert(autocomplete, command_info()->name, 1);
    list_add_last(commands, command_link());
    autocomplete = trie_insert(autocomplete, command_link()->name, 1);
    list_add_last(commands, command_list());
    autocomplete = trie_insert(autocomplete, command_list()->name, 1);
    list_add_last(commands, command_switch());
    autocomplete = trie_insert(autocomplete, command_switch()->name, 1);
}

void command_tini(void) {
    free_list(commands);
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
#include "device/control/device_controller.h"
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
                command_print(data);
                return CLI_CONTINUE;
            }
            /* Execute Command */
            return data->execute(args);
        }
    }

    return -1;
}

void command_print_all(void) {
    Command *data;
    if (commands == NULL) return;

    list_for_each(data, commands) {
        command_print(data);
    }
}

void command_print(const Command *command) {
    print_color(COLOR_YELLOW, "\t%-*s", COMMAND_SYNTAX_LENGTH, command->syntax);
    println("%s", command->description);
}

char *command_autocomplete_search(char *buffer, char *dat) {
    return trie_search(autocomplete->root, buffer, dat);
}