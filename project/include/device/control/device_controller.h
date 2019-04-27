
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
 * Check if the Controller has devices
 * @return true if has devices, false otherwise
 */
bool controller_has_devices(void);

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

/**
 * Show info about all devices
 */
void controller_info_all(void);
/**
 * Given a id, returns info on the device
 * @param id The Device id
 */
void controller_info_by_id(size_t id);
/**
 * Given an ID, set the switch name with switch_value
 * @param id child ID
 * @param switch_name switch name
 * @param switch_value switch value
 * @return true is successful
 */
int set_device_switch(size_t id, char switch_name[], char switch_value[]);

#endif
