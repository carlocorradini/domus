
#ifndef _DEVICE_H
#define _DEVICE_H

#include <unistd.h>
#include <stdbool.h>
#include "collection/collection_list.h"

#define DEVICE_PATH "./device/"
#define DEVICE_STATE true
#define DEVICE_NAME_LENGTH 15
#define DEVICE_DESCRIPTION_LENGTH 35
#define DEVICE_PATH_LENGTH 256
#define DEVICE_SWITCH_NAME_LENGTH 256

#define DEVICE_TYPE_BULB 0
#define DEVICE_TYPE_WINDOW 1
#define DEVICE_TYPE_FRIDGE 2
#define DEVICE_TYPE_CONTROLLER 3
#define DEVICE_TYPE_HUB 4
#define DEVICE_TYPE_TIMER 5


/**
 * Struct Device Descriptor,
 *  Information About a Device
 */
typedef struct DeviceDescriptor {
    char name[DEVICE_NAME_LENGTH];
    char description[DEVICE_DESCRIPTION_LENGTH];
    char file_name[DEVICE_PATH_LENGTH];

    int type;
} DeviceDescriptor;

/**
 * Struct generic switch
 */
typedef struct DeviceSwitch {
    char name[DEVICE_SWITCH_NAME_LENGTH];
    void *state;

    bool (*set_state)(char name[], void *state);
} DeviceSwitch;


/**
 * Struct generic Device
 */
typedef struct Device {
    size_t id;

    pid_t pid;
    bool state;
    void *registry;

    List *switches;

    DeviceDescriptor *device_descriptor;
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
 * Initialize the List of supported Devices
 */
void device_init(void);

/**
 * Free the List of supported Devices
 */
void device_tini(void);

/**
 * Create a new generic Device
 * @param pid Device process id
 * @param id Device unique id
 * @param state Device state
 * @param registry Device registry
 * @param master_switch Function to change state and make custom operations
 * @return The new Device, NULL otherwise
 */
Device *new_device(pid_t pid, size_t id, bool state, void *registry);

/**
 * Free a Device
 * @param device The Device to free
 * @return true if the Device has been freed, false otherwise
 */
bool free_device(Device *device);

/**
 * Create a new device switch
 * @param name switch name
 * @param state switch state
 * @param set_state method that set the state to the device
 * @return the created switch
 */
DeviceSwitch *new_device_switch(char name[], void *state, bool  (*set_state)(void *state));


/**
 * Check if a Device is correctly initialized
 * @param device The Device to check
 * @return true if correctly initialized, false otherwise
 */
bool device_check_device(const Device *device);

/**
 * Create a new generic Control Device
 * @param device The generic Device
 * @param devices The List for all controlled Devices
 * @return The new ControlDevice, NULL otherwise
 */
ControlDevice *new_control_device(Device *device, List *devices);

/**
 * Free a Control Device
 * @param control_device The Control Device to free
 * @return true if the Control Device has been freed, false otherwise
 */
bool free_control_device(ControlDevice *control_device);

/**
 * Check if a Control Device is correctly initialized
 * @param control_device The Control Device to check
 * @return true if correctly initialized, false otherwise
 */
bool device_check_control_device(const ControlDevice *control_device);

/**
 * Create a new Device Descriptor
 * @param name Name of the Device
 * @param description Description of the Device
 * @param file_name The name of the Device binaries
 * @return The new DeviceDescriptor
 */
DeviceDescriptor *new_device_descriptor(char name[], char description[], char file_name[]);

/**
 * Check if a device is supported,
 *  if found return the Device Descriptor, NULL otherwise
 * @param device The device to check
 * @return The Device Descriptor, NULL otherwise
 */
DeviceDescriptor *device_is_supported(const char *device);

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
