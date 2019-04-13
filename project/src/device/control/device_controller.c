
#include "device/device.h"
#include "device/control/device_controller.h"
#include "command/command.h"
#include "author.h"
#include "cli.h"

/**
 * Controller that must be defined and it cannot be visible outside this file
 * Only one can exist!!!
 */
static Device controller = {CONTROLLER_STATE, CONTROLLER_MASTER_SWITCH, CONTROLLER_REGISTRY};

/**
 * Initialize all Controller Components
 */
static void controller_init(void);

/**
 * Free all Controller Components
 */
static void controller_free(void);

void controller_start(void) {
    controller_init();
    cli_start();
    controller_free();
}

static void controller_init(void) {
    commands_init();
    authors_init();
}

static void controller_free(void) {
    commands_free();
    authors_free();
}