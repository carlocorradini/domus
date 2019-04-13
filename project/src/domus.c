
#include "domus.h"
#include "author.h"
#include "command/command.h"
#include "printer.h"
#include "cli.h"

/**
 * Initialize all Domus Components
 */
static void domus_init(void);

/**
 * Free all Domus Components
 */
static void domus_free(void);

/**
 * Show information about Domus
 */
static void domus_information(void);

/**
 * Domus Welcome Page
 */
static void domus_welcome(void);

void domus_start(void) {
    domus_init();
    domus_welcome();
    cli_start();
    domus_free();
}

static void domus_init(void) {
    authors_init();
    commands_init();
}

static void domus_free(void) {
    authors_free();
    commands_free();
}

static void domus_information(void) {
    println_color(COLOR_BLUE, "-> INFORMATION <-");
    println_color(COLOR_MAGENTA, "- AUTHORS");
    author_print_all();
    println_color(COLOR_MAGENTA, "- DESCRIPTION");
    println("\t%s", DOMUS_DESCRIPTION);
    println_color(COLOR_MAGENTA, "- VERSION");
    println("\t%s", DOMUS_VERSION);
    println_color(COLOR_MAGENTA, "- LICENSE");
    println("\t%s", DOMUS_LICENSE);
}

static void domus_welcome(void) {
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