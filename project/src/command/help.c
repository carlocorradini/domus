
#include "command/help.h"
#include "cli.h"

/**
 * Display help information about Domus
 * @param args Arguments
 * @return CLI status code
 */
static int _help(char **args) {
    Command *command;
    if (commands == NULL) return CLI_CONTINUE;

    list_for_each(item, commands)
    {
        command = (Command *) item->data;
        command_information(command);
    }

    return CLI_CONTINUE;
}

Command *command_help(void) {
    return new_command(
            "help",
            "Display help information about Domus",
            "help",
            _help);
}