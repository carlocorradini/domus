
#include <stdbool.h>
#include "cli/cli.h"
#include "cli/command/command_clear.h"

static bool is_unix = true;
#ifdef defined(_WIN32) || defined(_WIN64)
is_unix = false;
#else
#ifdef defined(__linux)
is_unix = true;
#endif
#endif

/**
 * Clear the CLI interface
 * @param args Arguments
 * @return CLI status code
 */
static int _clear(char **args) {
    (is_unix) ? system("clear") : system("CLS");
    return CLI_CONTINUE;
}

Command *command_clear(void) {
    return new_command(
            "clear",
            "Clear the CLI interface",
            "clear",
            _clear);
}