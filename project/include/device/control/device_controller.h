
#ifndef _DEVICE_CONTROLLER_H
#define _DEVICE_CONTROLLER_H

#include <stdbool.h>

#define CONTROLLER_STATE true
#define CONTROLLER_MASTER_SWITCH true
#define CONTROLLER_REGISTRY 0

/**
 * Start the Master Controller
 */
void controller_start(void);

#endif
