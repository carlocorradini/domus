
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "device/device.h"

/**
 * The List of Supported Devices
 */
static List *supported_devices = NULL;

void device_init(void) {
    if (supported_devices != NULL) return;
    supported_devices = new_list(NULL, NULL);

    list_add_last(supported_devices, new_device_descriptor("bulb", "A Simple Bulb", "bulb"));
    list_add_last(supported_devices, new_device_descriptor("hub", "A Simple Hub", "hub"));
    list_add_last(supported_devices, new_device_descriptor("timer", "A Simple Timer", "timer"));
}

void device_tini(void) {
    free_list(supported_devices);
}

Device *new_device(pid_t PID, bool state, void *registry, bool (*master_switch)(bool state)) {
    Device *device;
    if (registry == NULL || master_switch == NULL) {
        fprintf(stderr, "Device: Please define all required function\n");
        return NULL;
    }
    device = (Device *) malloc(sizeof(Device));
    if (device == NULL) {
        perror("Device Memory Allocation");
        exit(EXIT_FAILURE);
    }

    device->PID = PID;
    device->state = state;
    device->registry = registry;
    device->master_switch = master_switch;

    return device;
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

DeviceDescriptor *new_device_descriptor(char name[], char description[], char file_name[]) {
    char real_path[DEVICE_PATH_LENGTH] = {};
    DeviceDescriptor *device_descriptor = (DeviceDescriptor *) malloc(sizeof(DeviceDescriptor));
    if (device_descriptor == NULL) {
        perror("DeviceDescriptor Memory Allocation");
        exit(EXIT_FAILURE);
    }

    strncpy(device_descriptor->name, name, DEVICE_NAME_LENGTH);
    strncpy(device_descriptor->description, description, DEVICE_DESCRIPTION_LENGTH);
#ifdef _WIN32
    strcat(file_name, ".exe");
#endif
    strcat(real_path, DEVICE_PATH);
    strcat(real_path, file_name);
    strncpy(device_descriptor->file_name, real_path, DEVICE_PATH_LENGTH);

    return device_descriptor;
}

bool device_change_state(Device *device, bool state) {
    if (device == NULL) return false;
    if (device->state == state) return true;
    return device->master_switch(state);
}

bool device_is_supported(const char *device) {
    DeviceDescriptor *data;
    if (device == NULL) return false;

    list_for_each(data, supported_devices) {
        if (strcmp(data->name, device) == 0)
            return true;
    }
    return false;
}