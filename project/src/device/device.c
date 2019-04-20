
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

Device *new_device(pid_t pid, size_t id, bool state, void *registry, bool (*master_switch)(bool)) {
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

    device->pid = pid;
    device->id = id;
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

bool free_control_device(ControlDevice *control_device) {
    if(control_device == NULL) return false;
    if(control_device->device == NULL || control_device->devices == NULL) return false;

    free_list(control_device->devices);
    free(control_device->device);
    free(control_device);

    return true;
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
    /* Get absolute path & attach the file*/
    getcwd(real_path, sizeof(real_path));
    strcat(real_path, DEVICE_PATH);
    strcat(real_path, file);
    strncpy(device_descriptor->file_name, real_path, DEVICE_PATH_LENGTH);

    return device_descriptor;
}

bool device_change_state(Device *device, bool state) {
    if (device == NULL) return false;
    if (device->state == state) return true;
    return device->master_switch(state);
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