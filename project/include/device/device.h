
#ifndef _DEVICE_H
#define _DEVICE_H

#include <unistd.h>
#include <stdbool.h>
#include "collection/collection_list.h"
#include "device/device_communication.h"

#define DEVICE_PATH "./device/"
#define DEVICE_STATE true
#define DEVICE_NAME_LENGTH 36
#define DEVICE_DESCRIPTION_LENGTH 128
#define DEVICE_PATH_LENGTH 64
#define DEVICE_SWITCH_NAME_LENGTH 256

#define DEVICE_TYPE_BULB 0
#define DEVICE_TYPE_WINDOW 1
#define DEVICE_TYPE_FRIDGE 2
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

    bool (*set_state)(const char *, void *);
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
DeviceSwitch *new_device_switch(char name[], void *state, bool  (*set_state)(const char *, void *));

/**
 * Get switch state from its name
 * @param switch_list list of switches
 * @param name name of the switches
 * @return pointer to switch
 */
void *get_device_switch_state(List *switch_list, char name[]);

/**
 * Get switch object from its name
 * @param switch_list list of switches
 * @param name name of the switches
 * @return pointer to switch
 */
DeviceSwitch *get_device_switch(List *switch_list, char name[]);

/**
 * Check if a Device is correctly initialized
 * @param device The Device to check
 * @return true if correctly initialized, false otherwise
 */
bool device_check_device(const Device *device);

/**
 * Create a new generic Control Device
 * @param device The generic Device
 * @return The new ControlDevice, NULL otherwise
 */
ControlDevice *new_control_device(Device *device);

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

/**
 * Create a new process using fork() and save it to the controller devices list
 *  The child process execute an exec and change itself
 * @param device_descriptor The descriptor of the device to add
 * @return ID of the newly created process, -1 otherwise
 */

/**
 * Create a new process using fork() and save it to the controller devices list
 *  The child process execute an exec and change itself
 * @param control_device The control device
 * @param id the child id
 * @param device_descriptor The Device Descriptor of the child
 * @return true if fork was successful, false otherwise
 */
bool control_device_fork(const ControlDevice *control_device, size_t id, const DeviceDescriptor *device_descriptor);

/**
 * Check if the Control Device has Devices
 * @param control_device The control device to check
 * @return true if has devices, false otherwise
 */
bool control_device_has_devices(const ControlDevice *control_device);

/**
 * Check if the id is a valid id
 * @param id The id to check
 * @param control_device The Control Device
 * @return true if is a valid id, false otherwise
 */
bool control_device_valid_id(size_t id, const ControlDevice *control_device);

/**
 * Return a DeviceCommunication given an id
 * @param id The Device id
 * @param control_device The Control Device to get from
 * @return The Device Communication, NULL otherwise
 */
struct DeviceCommunication *
control_device_get_device_communication_by_id(size_t id, const ControlDevice *control_device);

#endif
