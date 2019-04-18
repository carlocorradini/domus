
#include "device/control/device_controller.h"
#include "device/device.h"
#include "cli/command/command.h"
#include "cli/cli.h"
#include "author.h"

/**
 * Controller that must be defined and it cannot be visible outside this file
 * Only one can exist in the entire program!!!
 */
static ControlDevice *controller = NULL;

/**
 * Initialize all Controller Components
 */
static void controller_init(void);

/**
 * Free all Controller Components
 */
static void controller_tini(void);

/**
 * Change the state of the controller
 * @param state The state to change to
 * @return true if the operation was successful, false otherwise
 */
static bool controller_master_switch(bool state);

void controller_start(void) {
    controller_init();
    cli_start();
    controller_tini();
}

static void controller_init(void) {
    controller = new_control_device(
            new_device(getpid(),
                       DEVICE_STATE,
                       new_controller_registry(),
                       controller_master_switch),
            new_list(NULL, NULL));

    command_init();
    author_init();
    device_init();
}

static void controller_tini(void) {
    command_tini();
    author_tini();
    device_tini();
}

static bool controller_master_switch(bool state) {
    if (controller == NULL) return false;
    controller->device->state = state;
    return true;
}

ControllerRegistry *new_controller_registry(void) {
    ControllerRegistry *controller_registry = (ControllerRegistry *) malloc(sizeof(ControllerRegistry));
    if (controller_registry == NULL) {
        perror("Controller Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }

    controller_registry->num = 0;

    return controller_registry;
}