
#include <stdlib.h>
#include "device/manual_control.h"
#include "cli/cli.h"
#include "cli/command/command.h"
#include "util/util_printer.h"
#include "author.h"

#define DOMUS_MANUAL_VERSION "1.0.0"
#define DOMUS_MANUAL_LICENSE "MIT"
#define DOMUS_MANUAL_SLOGAN "Odino purchè temano"
#define DOMUS_MANUAL_DESCRIPTION "Home Automation at your finger"

/**
 * Show information about Domus Manual
 */
static void domus_manual_information(void);

/**
 * Domus Manual Welcome Page
 */
static void domus_manual_welcome(void);

int main(int argc, char **args) {
    domus_manual_welcome();
    manual_command_init();
    cli_start();

    return EXIT_SUCCESS;
}

static void domus_manual_information(void) {
    println_color(COLOR_YELLOW, "- AUTHORS");
    author_print_all();
    println_color(COLOR_YELLOW, "- DESCRIPTION");
    println("\t%s", DOMUS_MANUAL_DESCRIPTION);
    println_color(COLOR_YELLOW, "- VERSION");
    println("\t%s", DOMUS_MANUAL_VERSION);
    println_color(COLOR_YELLOW, "- LICENSE");
    println("\t%s", DOMUS_MANUAL_LICENSE);
}

static void domus_manual_welcome(void) {
    author_init();
    println("=================================================================");
    println(
            "      _                                                               _ \n"
            "     | |                                                             | |\n"
            "   __| | ___  _ __ ___  _   _ ___   _ __ ___   __ _ _ __  _   _  __ _| |\n"
            "  / _` |/ _ \\| '_ ` _ \\| | | / __| | '_ ` _ \\ / _` | '_ \\| | | |/ _` | |\n"
            " | (_| | (_) | | | | | | |_| \\__ \\ | | | | | | (_| | | | | |_| | (_| | |\n"
            "  \\__,_|\\___/|_| |_| |_|\\__,_|___/ |_| |_| |_|\\__,_|_| |_|\\__,_|\\__,_|_|"
    );
    println(" %s\n", DOMUS_MANUAL_SLOGAN);
    domus_manual_information();
    println("=================================================================");
}