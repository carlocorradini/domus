#ifndef _COMMAND_H
#define _COMMAND_H

#include "cli.h"

#define COMMAND_NAME_LENGTH 25
#define COMMAND_DESCRIPTION_LENGTH 250
#define COMMAND_SYNTAX_LENGTH 250

/**
 * Command Struct
 */
typedef struct Command {
    char name[COMMAND_NAME_LENGTH];
    char description[COMMAND_DESCRIPTION_LENGTH];
    char syntax[COMMAND_SYNTAX_LENGTH];

    int (*execute)(char **);
} Command;

/**
 * Create and return a List of supported Commands
 * @return The List of supported Commands
 */
List *command_get_all(void);

/**
 * Create a new Command
 * @param name Command name, command identifier
 * @param description Command description, help purpose
 * @param syntax Command syntax, help purpose
 * @param execute The function to execute when called
 * @return The new Command
 */
Command *new_command(char name[], char description[], char syntax[], int (*execute)(char **));

/**
 * Free a Command
 * @param command The command to free
 */
void free_command(Command *command);

#endif //_COMMAND_H
