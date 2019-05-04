
#ifndef _DEVICE_CONTROLLER_H
#define _DEVICE_CONTROLLER_H

#include <unistd.h>
#include <stdbool.h>
#include "device/device.h"

#define DEVICE_CONTROLLER_ID 0
#define DEVICE_CONTROLLER_DELETE_ALL_DEVICES -1
#define DEVICE_CONTROLLER_INFO_ALL_DEVICES -1

/**
 * Struct Controller Registry
 */
typedef struct ControllerRegistry {
    size_t next_id;
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
 * @return ID of the newly created process, -1 otherwise
 */
size_t controller_fork_device(const DeviceDescriptor *device_descriptor);

/**
 * Check if the controller has devices
 * @return true if has devices, false otherwise
 */
bool controller_has_devices(void);

/**
 * Show all connected devices and all information about them
 */
void controller_list(void);

/**
 * Delete a device given the id
 *  if id equals to macro DEVICE_CONTROLLER_DELETE_ALL_DEVICES then remove all devices in the system
 * @param id The id of the device to remove
 * @return true if removed, false otherwise
 */
bool controller_del_by_id(size_t id);

/**
 * Delete all devices passing the macro DEVICE_CONTROLLER_DELETE_ALL_DEVICES to controller_del_by_id
 * @return true if removed, false otherwise
 */
bool controller_del_all(void);

/**
 * Given an id, returns info of the device
 * @param id The Device id
 * @return true if found, false otherwise
 */
bool controller_info_by_id(size_t id);

/**
 * Show info about all devices
 * @return true if found, false otherwise
 */
bool controller_info_all(void);

/**
 * Given an ID, set the switch label to switch_pos
 * @param id The Device id
 * @param switch_label The Device Switch Label
 * @param switch_pos switch pos The Device Switch Position
 * @return 0 if successful, error value otherwise
 */
int controller_switch(size_t id, const char *switch_label, const char *switch_pos);

/**
 * Link a Device with a Control Device
 * @param device_id The device id
 * @param control_device_id  The Control device id
 * @return true if linked correctly, false otherwise
 */
bool controller_link(size_t device_id, size_t control_device_id);

#endif
