
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "device/device.h"
#include "device/device_child.h"
#include "util/util_printer.h"

/**
 * The List of Supported Devices
 */
static List *supported_devices = NULL;

/**
 *
 * @param data_1
 * @param data_2
 * @return
 */
static bool device_device_descriptor_equals(const DeviceDescriptor *data_1, const DeviceDescriptor *data_2);

/**
 * Compare two Device Communication
 * @param data_1 first Device Communication
 * @param data_2 second Device Communication
 * @return true if equals, false otherwise
 */
static bool device_device_communication_equals(const DeviceCommunication *data_1, const DeviceCommunication *data_2);

/**
 * Compare two Switch by name
 * @param data_1 first string
 * @param data_2 second string
 * @return true if equals, false otherwise
 */
static bool device_switch_equals(const char *data_1, const char *data_2);

/**
 * Replaces the current running process with a new device process described in the Device Descriptor
 * @param child_id The child id
 * @param device_descriptor The descriptor of the device to be created
 */
static void control_device_fork_child(size_t child_id, const DeviceDescriptor *device_descriptor);

static bool device_device_descriptor_equals(const DeviceDescriptor *data_1, const DeviceDescriptor *data_2) {
    if (data_1 == NULL || data_2 == NULL) return false;
    return data_1->id == data_2->id;
}

static bool device_device_communication_equals(const DeviceCommunication *data_1, const DeviceCommunication *data_2) {
    if (data_1 == NULL || data_2 == NULL) return false;
    return data_1->pid == data_2->pid;
}

static bool device_switch_equals(const char *data_1, const char *data_2) {
    if (data_1 == NULL || data_2 == NULL) return false;
    return strcmp(data_1, data_2) == 0;
}

void device_init(void) {
    if (supported_devices != NULL) return;
    supported_devices = new_list(NULL, device_device_descriptor_equals);

    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_CONTROLLER, true, "controller",
                                                           "The Master Controller",
                                                           "NO_FILE_NAME"));
    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_BULB, false, "bulb",
                                                           "An electric light with a wire filament heated to such a high temperature that it glows with visible light",
                                                           "./device/bulb"));
    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_WINDOW, false, "window",
                                                           "An opening in a wall, door, roof or vehicle that allows the passage of light, sound, and air",
                                                           "./device/window"));
    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_FRIDGE, false, "fridge",
                                                           "An appliance or compartment which is artificially kept cool and used to store food and drink. ",
                                                           "./device/fridge"));
    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_HUB, true, "hub",
                                                           "A Hub is a device for connecting multiple devices together and making them act as a single segment",
                                                           "./device/hub"));
    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_TIMER, true, "timer",
                                                           "An automatic mechanism for activating a device at a preset time.",
                                                           "./device/timer"));
}

void device_tini(void) {
    free_list(supported_devices);
}

Device *new_device(size_t device_id, size_t device_descriptor_id, bool state, void *registry) {
    Device *device;
    if (registry == NULL) {
        fprintf(stderr, "Device: Please define all required function\n");
        return NULL;
    }
    /* Init Supported Devices if not */
    device_init();

    device = (Device *) malloc(sizeof(Device));
    if (device == NULL) {
        perror("Device Memory Allocation");
        exit(EXIT_FAILURE);
    }

    device->id = device_id;
    if ((device->device_descriptor = device_is_supported_by_id(device_descriptor_id)) == NULL) {
        fprintf(stderr, "Device: Device Descriptor id not found\n");
        return NULL;
    }
    device->state = state;
    device->registry = registry;
    device->switches = new_list(NULL, device_switch_equals);

    return device;
}

bool free_device(Device *device) {
    if (device == NULL) return false;
    if (device->registry == NULL) return false;

    free(device->registry);
    free_list(device->switches);
    free(device);

    return true;
}

ControlDevice *new_control_device(Device *device) {
    ControlDevice *control_device;
    if (device == NULL) {
        fprintf(stderr, "ControlDevice: Please define all required structure\n");
        return NULL;
    }

    control_device = (ControlDevice *) malloc(sizeof(ControlDevice));
    if (control_device == NULL) {
        perror("ControlDevice Memory Allocation");
        exit(EXIT_FAILURE);
    }

    control_device->device = device;
    control_device->devices = new_list(NULL, device_device_communication_equals);

    return control_device;
}

bool free_control_device(ControlDevice *control_device) {
    if (control_device == NULL) return false;
    if (control_device->device == NULL || control_device->devices == NULL) return false;

    free_device(control_device->device);
    free_list(control_device->devices);
    free(control_device);

    return true;
}

DeviceDescriptor *
new_device_descriptor(size_t id, bool control_device, char name[], char description[], char file_name[]) {
    DeviceDescriptor *device_descriptor = (DeviceDescriptor *) malloc(sizeof(DeviceDescriptor));
    if (device_descriptor == NULL) {
        perror("DeviceDescriptor Memory Allocation");
        exit(EXIT_FAILURE);
    }

    device_descriptor->id = id;
    device_descriptor->control_device = control_device;
    strncpy(device_descriptor->name, name, DEVICE_NAME_LENGTH);
    strncpy(device_descriptor->description, description, DEVICE_DESCRIPTION_LENGTH);
    strncpy(device_descriptor->file_name, file_name, DEVICE_FILE_NAME_LENGTH);

    return device_descriptor;
}

DeviceDescriptor *device_is_supported_by_name(const char *device) {
    DeviceDescriptor *data;
    if (supported_devices == NULL) return false;
    if (device == NULL) return NULL;

    list_for_each(data, supported_devices) {
        if (strcmp(data->name, device) == 0)
            return data;
    }
    return NULL;
}

DeviceDescriptor *device_is_supported_by_id(size_t id) {
    DeviceDescriptor *data;
    if (supported_devices == NULL) return false;

    list_for_each(data, supported_devices) {
        if (data->id == id) return data;
    }

    return NULL;
}

DeviceSwitch *new_device_switch(char name[], void *state, bool  (*set_state)(const char *, void *)) {
    DeviceSwitch *device_switch = (DeviceSwitch *) malloc(sizeof(DeviceSwitch));
    if (device_switch == NULL) {
        perror("DeviceSwitch Memory Allocation");
        exit(EXIT_FAILURE);
    }

    strncpy(device_switch->name, name, DEVICE_SWITCH_NAME_LENGTH);
    device_switch->state = state;
    device_switch->set_state = set_state;

    return device_switch;
}

void *device_get_device_switch_state(const List *switch_list, char name[]) {
    return ((DeviceSwitch *) list_get(switch_list, list_get_index(switch_list, name)))->state;
}

DeviceSwitch *device_get_device_switch(const List *switch_list, char name[]) {
    return list_get(switch_list, list_get_index(switch_list, name));
}

bool device_check_device(const Device *device) {
    return device != NULL && device->registry != NULL;
}

bool device_check_control_device(const ControlDevice *control_device) {
    return control_device != NULL && device_check_device(control_device->device) && control_device->devices != NULL;
}

bool control_device_fork(const ControlDevice *control_device, size_t id, const DeviceDescriptor *device_descriptor) {
    pid_t child_pid;
    int write_parent_read_child[2];
    int write_child_read_parent[2];
    if (!device_check_control_device(control_device) || device_descriptor == NULL) return false;
    if (id < 0) return false;

    if (pipe(write_parent_read_child) == -1
        || pipe(write_child_read_parent) == -1) {
        perror("Control Device Fork Pipe");
        exit(EXIT_FAILURE);
    }

    /* Fork the current process */
    switch (child_pid = fork()) {
        case -1: {
            perror("Control Device Fork Forking");
            exit(EXIT_FAILURE);
        }
        case 0: {
            close(write_parent_read_child[1]);
            close(write_child_read_parent[0]);

            /* Attach child stdout to write child pipe */
            dup2(write_child_read_parent[1], DEVICE_COMMUNICATION_CHILD_WRITE);
            /* Attach child stdin to read child pipe */
            dup2(write_parent_read_child[0], DEVICE_COMMUNICATION_CHILD_READ);

            control_device_fork_child(id, device_descriptor);
            break;
        }
        default: {
            close(write_parent_read_child[0]);
            close(write_child_read_parent[1]);

            list_add_last(control_device->devices,
                          new_device_communication(child_pid, write_child_read_parent[0], write_parent_read_child[1]));

            break;
        }
    }

    return true;
}

static void control_device_fork_child(size_t child_id, const DeviceDescriptor *device_descriptor) {
    char *device_args[DEVICE_CHILD_ARGS_LENGTH + 1];
    char device_id[sizeof(size_t) + 1];
    char device_descriptor_id[sizeof(size_t) + 1];
    if (device_descriptor == NULL) return;

    snprintf(device_id, sizeof(size_t) + 1, "%ld", child_id);
    snprintf(device_descriptor_id, sizeof(size_t) + 1, "%ld", device_descriptor->id);

    device_args[0] = device_id;
    device_args[1] = device_descriptor_id;
    device_args[2] = NULL;

    if (execv(device_descriptor->file_name, device_args) == -1) {
        perror("Error exec Controller Fork Child");
        exit(EXIT_FAILURE);
    }
}

bool control_device_has_devices(const ControlDevice *control_device) {
    if (!device_check_control_device(control_device)) return false;
    return !list_is_empty(control_device->devices);
}

void device_print_all(void) {
    DeviceDescriptor *data;
    if (supported_devices == NULL) return;

    list_for_each(data, supported_devices) {
        device_print(data);
    }
}

void device_print(const DeviceDescriptor *device_descriptor) {
    print_color(COLOR_YELLOW, "\t%-*s", DEVICE_NAME_LENGTH, device_descriptor->name);
    println("%s", device_descriptor->description);
}