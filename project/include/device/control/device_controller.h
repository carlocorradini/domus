
#ifndef _DEVICE_CONTROLLER_H
#define _DEVICE_CONTROLLER_H

#include <stdbool.h>
#include <unistd.h>

#define CONTROLLER_STATE true
#define CONTROLLER_MASTER_SWITCH true
#define CONTROLLER_REGISTRY 0

#define DEVICE_BULB 1

/**
 * Start the Master Controller
 */
void controller_start(void);

void controller_process_spawn(int device);
#endif
