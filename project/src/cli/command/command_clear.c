
#include <stdbool.h>
#include "cli/cli.h"
#include "cli/command/command_clear.h"

#if defined(_WIN32) || defined(_WIN64)
    const bool os = false;
#else
#ifdef __linux
    const bool os = true;
#endif
#endif

/**
 * Clear the CLI interface
 * @param args Arguments
 * @return CLI status code
 */
static int _clear(char **args) {
    if(os){
        system("clear");
    }
    else{
        system("cls");
    }
    return CLI_CONTINUE;
}

Command *command_clear(void) {
    return new_command(
            "clear",
            "Clear the CLI interface",
            "clear",
            _clear);
}