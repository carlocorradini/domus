
#ifndef _DOMUS_H
#define _DOMUS_H

#include <unistd.h>
#include <stdbool.h>
#include "device/device.h"

#define DOMUS_ID 0
#define CONTROLLER_ID 1
#define DEVICE_MESSAGE_TO_ALL_DEVICES -1

/**
 * Struct Domus Registry
 */
typedef struct DomusRegistry {
    size_t next_id;
} DomusRegistry;

/**
 * Start Domus System
 */
void domus_start(void);

/**
 * Create a new Domus Registry
 * @return The new Domus Registry
 */
DomusRegistry *new_domus_registry(void);

/**
 * Create a new process using fork() and save it to the Domus devices list
 *  The Device can have a custom name
 *  The child process execute and change itself
 * @param device_descriptor The descriptor of the device to add
 * @param custom_name The custom name, can be NULL
 * @return ID of the newly created process, -1 otherwise
 */
size_t domus_fork_device(const DeviceDescriptor *device_descriptor, const char *custom_name);

/**
 * Check if the System is UP and Running
 * @return true if the Controller is active, false otherwise
 */
bool domus_system_is_active(void);

/**
 * Check if the Domus has devices
 * @return true if has devices, false otherwise
 */
bool domus_has_devices(void);

/**
 * Show all connected devices and all information about them
 */
void domus_list(void);

/**
 * Delete a device given the id
 *  If it's a Control Device delete is done recursively
 * @param id The id of the device to remove
 * @return true if removed, false otherwise
 */
bool domus_del_by_id(size_t id);

/**
 * Delete all Devices in Domus
 * @return true if removed, false otherwise
 */
bool domus_del_all(void);

/**
 * Given an id, returns info of the device
 *  If it's a Control Device show info about all connected devices
 * @param id The Device id
 * @return true if found, false otherwise
 */
bool domus_info_by_id(size_t id);

/**
 * Show info about all devices
 * @return true if found, false otherwise
 */
bool domus_info_all(void);

/**
 * Given an ID, set the switch label to switch_pos
 * @param id The Device id
 * @param switch_label The Device Switch Label
 * @param switch_pos switch pos The Device Switch Position
 */
void domus_switch(size_t id, const char *switch_label, const char *switch_pos);

/**
 * Link a Device with a Control Device
 * @param device_id The device id
 * @param control_device_id  The Control device id
 * @return 0 if success
 *          1 if No Device Found
 *          2 if No Control Device Found
 *          3 Control Device Error
 */
int domus_link(size_t device_id, size_t control_device_id);

/**
 * Return the pid of the given Device ID
 * @param device_id The Device ID to get pid from
 * @return The pid of the Device,
 */
pid_t domus_getpid(size_t device_id);

/**
 * Display the current devices hierarchy in the system
 */
void domus_hierarchy(void);

#endif
