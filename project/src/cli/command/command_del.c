
#include <stdio.h>
#include <errno.h>
#include "cli/cli.h"
#include "cli/command/command_del.h"
#include "util/util_printer.h"
#include "device/control/device_controller.h"

/**
 * Delete the device with id from the system
 *  If it's a control device, deletion is done recursively
 * @param args Arguments
 * @return CLI status code
 */
static int _del(char **args) {
    size_t id;
    const char *id_str;
    char *id_str_end = NULL;

    id_str = args[0];
    id_str_end = NULL;
    errno = 0;
    id = strtol(id_str, &id_str_end, 10);

    if (id_str == id_str_end) {
        println("\tNo digits found");
    } else if (errno == EINVAL) {
        println("\tBase contains unsupported value");
    } else if (errno != 0 && id == 0) {
        println("\tUnspecified error occurred");
    } else if (errno == 0 && id_str && *id_str_end != 0) {
        println("\tAdditional characters remain");
    } else if (id <= 0) {
        println("\tId %ld is invalid", id);
    }

    controller_del(id);

    return CLI_CONTINUE;
}

Command *command_del(void) {
    return new_command(
            "del",
            "Delete the device with <id> from the system. If it's a control device, deletion is done recursively",
            "del <id>",
            _del);
}