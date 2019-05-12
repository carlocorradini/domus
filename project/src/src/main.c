
#include <stdlib.h>
#include "domus.h"
#include "author.h"
#include "util/util_printer.h"

#define DOMUS_VERSION "1.0.0"
#define DOMUS_LICENSE "MIT"
#define DOMUS_SLOGAN "Unicuique sua domus nota"
#define DOMUS_DESCRIPTION "Home Automation at your CLI"

/**
 * Show information about Domus
 */
static void domus_information(void);

/**
 * Domus Welcome Page
 */
static void domus_welcome(void);

int main(int argc, char **args) {
    /* Show Domus Welcome Page */
    domus_welcome();
    /* Start Domus System */
    domus_start();

    return EXIT_SUCCESS;
}

static void domus_information(void) {
    println_color(COLOR_YELLOW, "- AUTHORS");
    author_print_all();
    println_color(COLOR_YELLOW, "- DESCRIPTION");
    println("\t%s", DOMUS_DESCRIPTION);
    println_color(COLOR_YELLOW, "- VERSION");
    println("\t%s", DOMUS_VERSION);
    println_color(COLOR_YELLOW, "- LICENSE");
    println("\t%s", DOMUS_LICENSE);
}

static void domus_welcome(void) {
    author_init();
    println("=================================================================");
    println(
            " _______    ______   .___  ___.  __    __       _______.\n"
            "|       \\  /  __  \\  |   \\/   | |  |  |  |     /       |\n"
            "|  .--.  ||  |  |  | |  \\  /  | |  |  |  |    |   (----`\n"
            "|  |  |  ||  |  |  | |  |\\/|  | |  |  |  |     \\   \\    \n"
            "|  '--'  ||  `--'  | |  |  |  | |  `--'  | .----)   |   \n"
            "|_______/  \\______/  |__|  |__|  \\______/  |_______/"
    );
    println("%s\n", DOMUS_SLOGAN);
    domus_information();
    println("=================================================================");
}