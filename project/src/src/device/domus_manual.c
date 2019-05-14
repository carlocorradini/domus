
#include <stdlib.h>
#include "device/domus_manual.h"
#include "cli/cli.h"
#include "cli/command/command.h"
#include "util/util_printer.h"
#include "author.h"
#include "device/device_communication.h"

#define DOMUS_MANUAL_VERSION "1.0.0"
#define DOMUS_MANUAL_LICENSE "MIT"
#define DOMUS_MANUAL_SLOGAN "Oderint dum metuant"
#define DOMUS_MANUAL_DESCRIPTION "Home Automation at your hand"

/**
 * Show information about Domus Manual
 */
static void domus_manual_information(void);

/**
 * Domus Manual Welcome Page
 */
static void domus_manual_welcome(void);


int main(int argc, char **args) {
    queue_message_create_queue();
    device_init();
    domus_manual_welcome();
    manual_command_init();
    cli_start();
    command_tini();

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
            " ________  ________  _____ ______   ___  ___  ________           _____ ______   ________  ________   ___  ___  ________  ___          \n"
            "|\\   ___ \\|\\   __  \\|\\   _ \\  _   \\|\\  \\|\\  \\|\\   ____\\         |\\   _ \\  _   \\|\\   __  \\|\\   ___  \\|\\  \\|\\  \\|\\   __  \\|\\  \\         \n"
            "\\ \\  \\_|\\ \\ \\  \\|\\  \\ \\  \\\\\\__\\ \\  \\ \\  \\\\\\  \\ \\  \\___|_        \\ \\  \\\\\\__\\ \\  \\ \\  \\|\\  \\ \\  \\\\ \\  \\ \\  \\\\\\  \\ \\  \\|\\  \\ \\  \\        \n"
            " \\ \\  \\ \\\\ \\ \\  \\\\\\  \\ \\  \\\\|__| \\  \\ \\  \\\\\\  \\ \\_____  \\        \\ \\  \\\\|__| \\  \\ \\   __  \\ \\  \\\\ \\  \\ \\  \\\\\\  \\ \\   __  \\ \\  \\       \n"
            "  \\ \\  \\_\\\\ \\ \\  \\\\\\  \\ \\  \\    \\ \\  \\ \\  \\\\\\  \\|____|\\  \\        \\ \\  \\    \\ \\  \\ \\  \\ \\  \\ \\  \\\\ \\  \\ \\  \\\\\\  \\ \\  \\ \\  \\ \\  \\____  \n"
            "   \\ \\_______\\ \\_______\\ \\__\\    \\ \\__\\ \\_______\\____\\_\\  \\        \\ \\__\\    \\ \\__\\ \\__\\ \\__\\ \\__\\\\ \\__\\ \\_______\\ \\__\\ \\__\\ \\_______\\\n"
            "    \\|_______|\\|_______|\\|__|     \\|__|\\|_______|\\_________\\        \\|__|     \\|__|\\|__|\\|__|\\|__| \\|__|\\|_______|\\|__|\\|__|\\|_______|"
    );
    println("");
    println(" %s\n", DOMUS_MANUAL_SLOGAN);
    domus_manual_information();
    println("=================================================================");
}