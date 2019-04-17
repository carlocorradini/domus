
#include "cli/cli.h"
#include "cli/command/command_help.h"
#include "collection/collection_linked_list.h"

/**
 * Display help information about Domus
 * @param args Arguments
 * @return CLI status code
 */
static int _help(char **args) {
    LinkedList *commands = command_get_commands();
    Command *data;
    if (commands == NULL) return CLI_CONTINUE;

    list_for_each(data, commands) {
        command_information(data);
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