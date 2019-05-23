
#ifndef _DEVICE_CONTROLLER_H
#define _DEVICE_CONTROLLER_H

#include <unistd.h>

#define CONTROLLER_SWITCH_STATE "switch"
#define CONTROLLER_SWITCH_STATE_ON "on"
#define CONTROLLER_SWITCH_STATE_OFF "off"
/**
 * Struct Controller Registry
 */
typedef struct ControllerRegistry {
    size_t directly_connected_devices;
} ControllerRegistry;

/**
 * Create a new Controller Registry
 * @return The new Controller Registry
 */
ControllerRegistry *new_controller_registry(void);

#endif
