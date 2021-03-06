#ifndef _COMMAND_H
#define _COMMAND_H

#define COMMAND_NAME_LENGTH 25
#define COMMAND_DESCRIPTION_LENGTH 250
#define COMMAND_SYNTAX_LENGTH 35

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
 * Initialize the List of supported Commands
 */
void command_init(void);

/**
 * Initialize the List of supported Commands for
 * Manual Control
 */
void manual_command_init(void);

/**
 * Free the List of supported Commands
 */
void command_tini(void);

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
 * Execute the command passed in args[0]
 * @param args Command & parameter/s
 * @return -1 if no command found or command return value
 */
int command_execute(char **args);

/**
 * Print all commands using command_print function
 */
void command_print_all(void);

/**
 * Print information about a Command
 * @param command The command to retrieve information
 */
void command_print(const Command *command);

/**
 *
 * @param buffer
 * @param dat
 * @return
 */
char *command_autocomplete_search(char *buffer, char *dat);

#endif
