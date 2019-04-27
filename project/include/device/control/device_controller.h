
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
bool controller_del_by_id(size_t id);

/**
 * Delete all devices
 * @return true if removed, false otherwise
 */
bool controller_del_all(void);

/**
 * Check if the id is a valid id
 * @param id The id to check
 * @return true if valid, false otherwise
 */
bool controller_valid_id(size_t id);

/**
 * Show info about all devices
 */
void controller_info_all(void);

/**
 * Given an id, returns info of the device
 * @param id The Device id
 */
void controller_command_info_by_id(size_t id);

/**
 * Given an ID, set the switch label to switch_pos
 * @param id The Device id
 * @param switch_label The Device Switch Label
 * @param switch_pos switch pos The Device Switch Position
 * @return 0 if successful, error value otherwise
 */
int controller_switch(size_t id, char *switch_label, char *switch_pos);

#endif
