
#ifndef _DEVICE_CONTROLLER_H
#define _DEVICE_CONTROLLER_H

#include <unistd.h>

/**
 * Struct Controller Registry
 */
typedef struct ControllerRegistry {
    size_t num;
} ControllerRegistry;

/**
 * Start the Master Controller
 */
void controller_start(void);

/**
 * Create a new Controller Registry
 * @return The new Controller Registry
 */
ControllerRegistry *new_controller_registry(void);

#endif
