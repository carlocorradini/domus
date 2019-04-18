
#ifndef _DEVICE_H
#define _DEVICE_H

#include <unistd.h>
#include <stdbool.h>
#include "collection/collection_list.h"

#define DEVICE_PATH "./device/"
#define DEVICE_STATE true
#define DEVICE_NAME_LENGTH 35
#define DEVICE_DESCRIPTION_LENGTH 250
#define DEVICE_PATH_LENGTH 100

/**
 * Struct generic Device
 */
typedef struct Device {
    pid_t PID;
    bool state;
    void *registry;

    bool (*master_switch)(bool state);
} Device;

/**
 * Struct control Device,
 *  a ControlDevice is A Device
 */
typedef struct ControlDevice {
    Device *device;
    List *devices;
} ControlDevice;

/**
 * Struct Device Descriptor,
 *  Information About a Device
 */
typedef struct DeviceDescriptor {
    char name[DEVICE_NAME_LENGTH];
    char description[DEVICE_DESCRIPTION_LENGTH];
    char file_name[DEVICE_PATH_LENGTH];
} DeviceDescriptor;

/**
 * Initialize the List of supported Devices
 */
void device_init(void);

/**
 * Free the List of supported Devices
 */
void device_tini(void);

/**
 * Create a new generic Device
 * @param PID Device process id
 * @param state Device state
 * @param registry Device registry
 * @param master_switch Function to change state and make custom operations
 * @return The new Device, NULL otherwise
 */
Device *new_device(pid_t PID, bool state, void *registry, bool (*master_switch)(bool state));

/**
 * Create a new generic ControlDevice
 * @param device The generic Device
 * @param devices The List for all controlled Devices
 * @return The new ControlDevice, NULL otherwise
 */
ControlDevice *new_control_device(Device *device, List *devices);

/**
 * Create a new Device Descriptor
 * @param name Name of the Device
 * @param description Description of the Device
 * @param file_name The name of the Device binaries
 * @return The new DeviceDescriptor
 */
DeviceDescriptor *new_device_descriptor(char name[], char description[], char file_name[]);

/**
 * Change the state of a Device calling it's custom function master_switch,
 *  if the operation was successful return true, false otherwise
 * @param device The Device to change state
 * @param state The state to change to
 * @return true if the operation was successful, false otherwise
 */
bool device_change_state(Device *device, bool state);

/**
 * Check if a device is supported or not
 * @param device The device to check
 * @return true if is supported, false otherwise
 */
bool device_is_supported(const char *device);

/**
 * Print all supported devices using device_print function
 */
void device_print_all(void);

/**
 * Print information about a Device
 * @param device_descriptor The device descriptor to retrieve information
 */
void device_print(const DeviceDescriptor *device_descriptor);

#endif
