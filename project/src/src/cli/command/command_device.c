
#include "cli/cli.h"
#include "cli/command/command_help.h"
#include "device/device.h"
#include "util/util_printer.h"

/**
 * Display supported devices in Domus
 * @param args Arguments
 * @return CLI status code
 */
static int _device(char **args) {
    device_print_legend();
    println_color(COLOR_GREEN, "\t%s» Switch is Automatic & Manual", COLOR_BOLD);
    println_color(COLOR_RED, "\t%s» Switch is Manual Only", COLOR_BOLD);
    println("\t%s~ Available switch position", COLOR_BOLD);
    println("");
    device_print_all();

    return CLI_CONTINUE;
}

Command *command_device(void) {
    return new_command(
            "device",
            "Display all supported devices and their description",
            "device",
            _device);
}