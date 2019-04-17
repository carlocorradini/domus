
#include "cli/cli.h"
#include "cli/command/command_add.h"
#include "device/control/device_controller.h"

/**
 * Add a device to the system and show its features
 * @param args Arguments
 * @return CLI status code
 */
static int _add(char **args) {

    if(args[1] != NULL && strcmp(args[1], "bulb")==0){
        printf("Creating new bulb\n");
        /*controller_process_spawn(1);*/
    }
    else{
        printf("\tDEVICE NOT SUPPORTED YET\n");
    }

    return CLI_CONTINUE;
}

Command *command_add(void) {
    return new_command(
            "add",
            "Add a <device> to the system and show its features",
            "add <device>",
            _add);
}