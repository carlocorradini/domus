
#include "cli/command/command_hierarchy.h"
#include "domus.h"
#include "cli/cli.h"
#include "util/util_printer.h"

/**
 * Display the current Device hierarchy in the system
 * @param args Arguments
 * @return CLI status code
 */
static int _hierarchy(char **args) {
    print("\t\x1b[46m SYSTEM %s", COLOR_RESET);
    print(" \x1b[43m CONTROL %s", COLOR_RESET);
    println(" \x1b[7m INTERACTION %s", COLOR_RESET);
    println("");

    domus_hierarchy();

    return CLI_CONTINUE;
}

Command *command_hierarchy(void) {
    return new_command(
            "hierarchy",
            "Display the current devices hierarchy in the system, described by <name> <id>",
            "hierarchy",
            _hierarchy);
}