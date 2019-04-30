
#include "domus.h"
#include "author.h"
#include "util/util_printer.h"
#include "device/control/device_controller.h"

/**
 * Show information about Domus
 */
static void domus_information(void);

/**
 * Domus Welcome Page
 */
static void domus_welcome(void);

void domus_start(void) {
    author_init();
    domus_welcome();
    controller_start();
}

static void domus_information(void) {
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