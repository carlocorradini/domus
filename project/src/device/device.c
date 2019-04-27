
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "device/device.h"
#include "device/device_child.h"
#include "util/util_printer.h"
#include "util/util_os.h"


/**
 * The List of Supported Devices
 */
static List *supported_devices = NULL;

/**
 * Compare the two Strings
 * @param data_1 first string
 * @param data_2 second string
 * @return true if the two Strings are equals, false otherwise
 */
static bool device_switch_equals(const char *data_1, const char *data_2);

/**
 * Methods to compare DeviceCommunication and id
 * @param data_1 DeviceCommunication data (element of controller->devices)
 * @param data_2 id
 * @return true if  equals, false otherwise
 */
static bool device_process_equals(const DeviceCommunication *data_1, const DeviceCommunication *data_2);

/**
 * Replaces the current running process with a new device process described in the Device Descriptor
 * @param child_id The child id
 * @param parent_id The parent id
 * @param device_descriptor The descriptor of the device to be created
 */
static void control_device_fork_child(size_t child_id, size_t parent_id, const DeviceDescriptor *device_descriptor);

void device_init(void) {
    if (supported_devices != NULL) return;
    supported_devices = new_list(NULL, NULL);

    list_add_last(supported_devices, new_device_descriptor("bulb",
                                                           "An electric light with a wire filament heated to such a high temperature that it glows with visible light",
                                                           "bulb"));
    list_add_last(supported_devices, new_device_descriptor("window",
                                                           "An opening in a wall, door, roof or vehicle that allows the passage of light, sound, and air",
                                                           "window"));
}

void device_tini(void) {
    free_list(supported_devices);
}

static bool device_switch_equals(const char *data_1, const char *data_2) {
    return strcmp(data_1, data_2) == 0;
}

static bool device_process_equals(const DeviceCommunication *data_1, const DeviceCommunication *data_2) {
    return data_1->id == data_2->id;
}

Device *new_device(pid_t pid, size_t id, bool state, void *registry) {
    Device *device;
    if (registry == NULL) {
        fprintf(stderr, "Device: Please define all required function\n");
        return NULL;
    }
    device = (Device *) malloc(sizeof(Device));
    if (device == NULL) {
        perror("Device Memory Allocation");
        exit(EXIT_FAILURE);
    }

    device->pid = pid;
    device->id = id;
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

bool device_check_device(const Device *device) {
    return device != NULL && device->registry != NULL;
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
    control_device->devices = new_list(NULL, device_process_equals);

    return control_device;
}

bool free_control_device(ControlDevice *control_device) {
    if (control_device == NULL) return false;
    if (control_device->device == NULL || control_device->devices == NULL) return false;

    free_list(control_device->devices);
    if (!free_device(control_device->device)) return false;
    free(control_device);

    return true;
}

bool device_check_control_device(const ControlDevice *control_device) {
    return control_device != NULL && device_check_device(control_device->device) && control_device->devices != NULL;
}

DeviceDescriptor *new_device_descriptor(char name[], char description[], char file_name[]) {
    char real_path[DEVICE_PATH_LENGTH];
    char file[DEVICE_PATH_LENGTH - DEVICE_NAME_LENGTH];
    DeviceDescriptor *device_descriptor = (DeviceDescriptor *) malloc(sizeof(DeviceDescriptor));
    if (device_descriptor == NULL) {
        perror("DeviceDescriptor Memory Allocation");
        exit(EXIT_FAILURE);
    }

    strncpy(device_descriptor->name, name, DEVICE_NAME_LENGTH);
    strncpy(device_descriptor->description, description, DEVICE_DESCRIPTION_LENGTH);
    /* Copy the file name to file */
    strncpy(file, file_name, DEVICE_PATH_LENGTH - DEVICE_NAME_LENGTH);
    /* Get path & attach the file*/
    strcpy(real_path, DEVICE_PATH);
    strcat(real_path, file);
    strncpy(device_descriptor->file_name, real_path, DEVICE_PATH_LENGTH);

    return device_descriptor;
}

DeviceDescriptor *device_is_supported(const char *device) {
    DeviceDescriptor *data;
    if (device == NULL) return NULL;

    list_for_each(data, supported_devices) {
        if (strcmp(data->name, device) == 0)
            return data;
    }
    return NULL;
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


/**********************************************************************************************************************/
bool control_device_fork(const ControlDevice *control_device, size_t id, const DeviceDescriptor *device_descriptor) {
    pid_t child_pid;
    size_t parent_id;
    int write_parent_read_child[2];
    int write_child_read_parent[2];
    if (!device_check_control_device(control_device) || device_descriptor == NULL) return false;
    if (id < 0) return false;

    if (pipe(write_parent_read_child) == -1
        || pipe(write_child_read_parent) == -1) {
        perror("Control Device Fork Pipe");
        exit(EXIT_FAILURE);
    }

    /* Get parent id */
    parent_id = control_device->device->id;
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

            control_device_fork_child(id, parent_id, device_descriptor);
            break;
        }
        default: {
            close(write_parent_read_child[0]);
            close(write_child_read_parent[1]);

            list_add_last(control_device->devices,
                          new_device_communication(id, child_pid, device_descriptor, write_child_read_parent[0],
                                                   write_parent_read_child[1]));

            break;
        }
    }

    return true;
}

static void control_device_fork_child(size_t child_id, size_t parent_id, const DeviceDescriptor *device_descriptor) {
    char *device_args[DEVICE_CHILD_ARGS_LENGTH + 1];
    char device_id[sizeof(size_t) + 1];
    char device_parent_id[sizeof(size_t) + 1];
    char device_name[DEVICE_NAME_LENGTH];
    char device_description[DEVICE_DESCRIPTION_LENGTH];
    if (device_descriptor == NULL) return;

    snprintf(device_id, sizeof(size_t) + 1, "%ld", child_id);
    snprintf(device_parent_id, sizeof(size_t) + 1, "%ld", parent_id);
    strncpy(device_name, device_descriptor->name, DEVICE_NAME_LENGTH);
    strncpy(device_description, device_descriptor->description, DEVICE_DESCRIPTION_LENGTH);

    device_args[0] = device_id;
    device_args[1] = device_parent_id;
    device_args[2] = device_name;
    device_args[3] = device_description;
    device_args[4] = NULL;

    if (execv(device_descriptor->file_name, device_args) == -1) {
        perror("Error exec Controller Fork Child");
        exit(EXIT_FAILURE);
    }
}

bool control_device_has_devices(const ControlDevice *control_device) {
    if (!device_check_control_device(control_device)) return false;
    return !list_is_empty(control_device->devices);
}

bool control_device_valid_id(size_t id, const ControlDevice *control_device) {
    DeviceCommunication fake_communication;
    if (!control_device_has_devices(control_device)) return false;
    if (id <= 0) return false;

    fake_communication.id = id;

    return list_contains(control_device->devices, &fake_communication);
}

DeviceCommunication *control_device_get_device_communication_by_id(size_t id, const ControlDevice *control_device) {
    DeviceCommunication fake_communication;
    if (!control_device_valid_id(id, control_device)) return NULL;

    fake_communication.id = id;

    return (DeviceCommunication *) list_get(control_device->devices,
                                            list_get_index(control_device->devices, &fake_communication));
}