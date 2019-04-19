
#ifndef _DEVICE_CONTROLLER_H
#define _DEVICE_CONTROLLER_H

#include <unistd.h>
#include <stdbool.h>
#include "device/device.h"

/**
 * Struct Controller Registry
 */
typedef struct ControllerRegistry {
    size_t connected_directly;
    size_t connected_total;
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

/**
 * Create a new process using fork() and save it to the controller devices list
 *  The child process execute an exec and change itself
 * @param device_descriptor The descriptor of the device to add
 * @return true if fork was successful, false otherwise
 */
bool controller_fork_device(const DeviceDescriptor *device_descriptor);

/**
 * Return the number of directly connected devices
 * @return directly connected devices, -1 otherwise
 */
size_t controller_connected_directly(void);

/**
 * Return the number of total connected devices
 * @return total connected devices, -1 otherwise
 */
size_t controller_connected_total(void);

#endif
