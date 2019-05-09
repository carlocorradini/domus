
#ifndef _DEVICE_CONTROLLER_H
#define _DEVICE_CONTROLLER_H

#include <unistd.h>

/**
 * Struct Controller Registry
 */
typedef struct ControllerRegistry {
} ControllerRegistry;

/**
 * Create a new Controller Registry
 * @return The new Controller Registry
 */
ControllerRegistry *new_controller_registry(void);

#endif
