
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "device/device.h"
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

ControlDevice *new_control_device(Device *device, List *devices) {
    ControlDevice *control_device;
    if (device == NULL || devices == NULL) {
        fprintf(stderr, "ControlDevice: Please define all required structure\n");
        return NULL;
    }
    control_device = (ControlDevice *) malloc(sizeof(ControlDevice));
    if (control_device == NULL) {
        perror("ControlDevice Memory Allocation");
        exit(EXIT_FAILURE);
    }

    control_device->device = device;
    control_device->devices = devices;

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