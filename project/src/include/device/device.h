
#ifndef _DEVICE_H
#define _DEVICE_H

#include <unistd.h>
#include <stdbool.h>
#include "collection/collection_list.h"

#define DEVICE_STATE true
#define DEVICE_NAME_LENGTH 16
#define DEVICE_DESCRIPTION_LENGTH 128
#define DEVICE_FILE_NAME_LENGTH 32
#define DEVICE_SWITCH_NAME_LENGTH 24
#define DEVICE_SWITCH_DESCRIPTION_LENGTH 128
#define DEVICE_STATE_LENGTH 16

#define DEVICE_TYPE_DOMUS 0
#define DEVICE_TYPE_CONTROLLER 1
#define DEVICE_TYPE_BULB 2
#define DEVICE_TYPE_WINDOW 3
#define DEVICE_TYPE_FRIDGE 4
#define DEVICE_TYPE_HUB 5
#define DEVICE_TYPE_TIMER 6

/**
 * Struct Device Descriptor,
 *  Information About a Device
 */
typedef struct DeviceDescriptor {
    size_t id;
    bool control_device;
    char name[DEVICE_NAME_LENGTH];
    char description[DEVICE_DESCRIPTION_LENGTH];
    char file_name[DEVICE_FILE_NAME_LENGTH];

    List *switches;
} DeviceDescriptor;

/**
 * Struct Device Descriptor Switch,
 *  Information about a Device Switch
 */
typedef struct DeviceDescriptorSwitch {
    char name[DEVICE_SWITCH_NAME_LENGTH];
    char description[DEVICE_SWITCH_DESCRIPTION_LENGTH];
    bool only_manual;

    List *positions;
} DeviceDescriptorSwitch;

typedef struct DeviceDescriptorSwitchPosition {
    char name[DEVICE_SWITCH_NAME_LENGTH];
    char description[DEVICE_SWITCH_DESCRIPTION_LENGTH];
} DeviceDescriptorSwitchPosition;

/**
 * Struct generic switch
 */
typedef struct DeviceSwitch {
    char name[DEVICE_SWITCH_NAME_LENGTH];
    void *state;

    int (*set_state)(const char *, void *);
} DeviceSwitch;

/**
 * Struct generic Device
 */
typedef struct Device {
    size_t id;
    DeviceDescriptor *device_descriptor;
    char name[DEVICE_NAME_LENGTH];
    bool state;
    void *registry;
    bool override;
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
 * @param device_id Device unique id
 * @param device_descriptor_id Device Descriptor unique id
 * @param name The Device Name
 * @param state Device state
 * @param registry Device registry
 * @return The new Device, NULL otherwise
 */
Device *new_device(size_t device_id, size_t device_descriptor_id, const char *name, bool state, void *registry);

/**
 * Free a Device
 * @param device The Device to free
 * @return true if the Device has been freed, false otherwise
 */
bool free_device(Device *device);

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
 * Create a new Device Descriptor
 * @param id The Device Descriptor id
 * @param control_device True if it is a Control Device
 * @param name Name of the Device
 * @param description Description of the Device
 * @param file_name The name of the Device binaries
 * @return The new DeviceDescriptor
 */
DeviceDescriptor *
new_device_descriptor(size_t id, bool control_device, char name[], char description[], char file_name[]);

/**
 * Create and return a new Device Descriptor Switch
 * @param name The name of the Switch
 * @param description The description of the Switch
 * @param only_manual Set to true if the Switch is available only in Manual
 * @return The new Device Descriptor Switch
 */
DeviceDescriptorSwitch *new_device_descriptor_switch(char name[], char description[], bool only_manual);

/**
 * Create and return a new Device Descriptor Switch Position
 * @param name The name of the Switch Position
 * @param description The description of the Switch Position
 * @return The new Device Descriptor Switch Position
 */
DeviceDescriptorSwitchPosition *new_device_descriptor_switch_position(char name[], char description[]);

/**
 * Add the new Switch to the Device, represented by a Device Descriptor
 *  Add the new Descriptor Switch to the last position of the List of the Device Descriptor
 * @param device_descriptor The Device Descriptor
 * @param name The name of the Switch
 * @param description The description of the Switch
 * @param only_manual Set to true if the Switch is available only in Manual
 * @return true if added, false otherwise
 */
bool device_device_descriptor_add_switch(DeviceDescriptor *device_descriptor, char name[], char description[],
                                         bool only_manual);

/**
 * Add the new Switch Position to the Device, represented by a Device Descriptor
 *  Add the new Descriptor Switch Position to the last position of the List of the Device Descriptor Switch
 * @param @param device_descriptor The Device Descriptor
 * @param name The name of the Switch
 * @param description The description of the Switch
 * @return true if added, false otherwise
 */
bool device_device_descriptor_add_position(DeviceDescriptor *device_descriptor, char *name,
                                           char *description);

/**
 * Check if a device is supported by name
 * @param device The Device Descriptor name to check
 * @return The Device Descriptor, NULL otherwise
 */
DeviceDescriptor *device_is_supported_by_name(const char *device);

/**
 * Check if a Device is supported by id
 * @param id The Device Descriptor id to check
 * @return The Device Descriptor, NULL otherwise
 */
DeviceDescriptor *device_is_supported_by_id(size_t id);

/**
 * Create a new device switch
 * @param name switch name
 * @param state switch state
 * @param set_state method that set the state to the device
 * @return the created switch
 */
DeviceSwitch *new_device_switch(char name[], void *state, int  (*set_state)(const char *, void *));

/**
 * Get switch state from its name
 * @param switch_list list of switches
 * @param name name of the switches
 * @return pointer to switch
 */
void *device_get_device_switch_state(const List *switch_list, const char *name);

/**
 * Get switch object from its name
 * @param switch_list list of switches
 * @param name name of the switches
 * @return pointer to switch
 */
DeviceSwitch *device_get_device_switch(const List *switch_list, const char *name);

/**
 * Check if a Device is correctly initialized
 * @param device The Device to check
 * @return true if correctly initialized, false otherwise
 */
bool device_check_device(const Device *device);

/**
 * Check if a Control Device is correctly initialized
 * @param control_device The Control Device to check
 * @return true if correctly initialized, false otherwise
 */
bool device_check_control_device(const ControlDevice *control_device);

/**
 * Create a new process using fork() and save it to the controller devices list
 *  The child process execute an exec and change itself
 *  The parent waits until child send a message with macro type 'I_AM_ALIVE'
 * @param control_device The control device
 * @param id the child id
 * @param device_descriptor The Device Descriptor of the child
 * @param custom_name The custom name, can be NULL
 * @return true if fork was successful, false otherwise
 */
bool control_device_fork(const ControlDevice *control_device, size_t id, const DeviceDescriptor *device_descriptor,
                         const char *custom_name);

/**
 * Check if the Control Device has Devices
 * @param control_device The control device to check
 * @return true if has devices, false otherwise
 */
bool control_device_has_devices(const ControlDevice *control_device);

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
 * Print the Domus Devices Legend
 */
void device_print_legend(void);

#endif
