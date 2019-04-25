
#ifndef _DEVICE_CONTROLLER_H
#define _DEVICE_CONTROLLER_H

#include <unistd.h>
#include <stdbool.h>
#include "device/device.h"

/**
 * Struct Controller Registry
 */
typedef struct ControllerRegistry {
    size_t next_id;
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
 * Show all connected devices and all information about them
 */
void controller_list(void);

/**
 * Delete a device given the id
 * @param id The id of the device to remove
 * @return true if removed, false otherwise
 */
bool controller_del(size_t id);

/**
 * Check if the id is present
 * @param id The id to check
 * @return The id if found, -1 otherwise
 */
size_t controller_valid_id(size_t id);

#endif
