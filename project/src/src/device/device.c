
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
 * Compare two DeviceDescriptor
 * @param data_1 first Device Descriptor
 * @param data_2 second Device Descriptor
 * @return true if equals, false otherwise
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
 * @param custom_name The custom name, can be NULL
 */
static void
control_device_fork_child(size_t child_id, const DeviceDescriptor *device_descriptor, const char *custom_name);

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
    supported_devices = new_list(NULL, (bool (*)(const void *, const void *)) device_device_descriptor_equals);

    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_DOMUS, true, "domus",
                                                           "Domus System",
                                                           "NO_FILE_NAME"));
    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_CONTROLLER, true, "controller",
                                                           "Domus Master Controller",
                                                           "./device/controller"));
    device_device_descriptor_add_switch(list_get_last(supported_devices), "system", "Turns on and off the Controller acting as a general master switch",
                                        true);
    device_device_descriptor_add_position(list_get_last(supported_devices), "on", "Turns on the Controller");
    device_device_descriptor_add_position(list_get_last(supported_devices), "off", "Turns off the Controller");
    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_HUB, true, "hub",
                                                           "A device for connecting multiple devices having the same type and making them act as a single segment",
                                                           "./device/hub"));
    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_TIMER, true, "timer",
                                                           "An automatic mechanism for activating a device at a preset time",
                                                           "./device/timer"));
    device_device_descriptor_add_switch(list_get_last(supported_devices), "time", "Set the timer", false);
    device_device_descriptor_add_position(list_get_last(supported_devices), "Y-m-d_H:i:s?Y-m-d_H:i:s",
                                          "The begin & end scheduling time divided by ?");
    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_BULB, false, "bulb",
                                                           "An electric light with a wire filament heated to such a high temperature that it glows with visible light",
                                                           "./device/bulb"));
    device_device_descriptor_add_switch(list_get_last(supported_devices), "turn", "Turns on and off the Bulb", false);
    device_device_descriptor_add_position(list_get_last(supported_devices), "on", "Turns on the Light");
    device_device_descriptor_add_position(list_get_last(supported_devices), "off", "Turns off the Light");
    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_WINDOW, false, "window",
                                                           "An opening in a wall, door, roof or vehicle that allows the passage of light, sound, and air",
                                                           "./device/window"));
    device_device_descriptor_add_switch(list_get_last(supported_devices), "open", "Open and Close the Window", false);
    device_device_descriptor_add_position(list_get_last(supported_devices), "on", "Open the window");
    device_device_descriptor_add_position(list_get_last(supported_devices), "off", "Close the window");
    list_add_last(supported_devices, new_device_descriptor(DEVICE_TYPE_FRIDGE, false, "fridge",
                                                           "An appliance or compartment which is artificially kept cool and used to store food and drink",
                                                           "./device/fridge"));
    device_device_descriptor_add_switch(list_get_last(supported_devices), "door", "Open and close the fridge's door",
                                        false);
    device_device_descriptor_add_position(list_get_last(supported_devices), "on", "Open the fridge's door");
    device_device_descriptor_add_position(list_get_last(supported_devices), "off", "Open the fridge's door");
    device_device_descriptor_add_switch(list_get_last(supported_devices), "thermo",
                                        "Set the internal temperature of the Fridge", false);
    device_device_descriptor_add_position(list_get_last(supported_devices), "<temp>",
                                          "Set the fridge's temperature to <temp>");
    device_device_descriptor_add_switch(list_get_last(supported_devices), "delay",
                                        "Set the delay until the door automatically close", false);
    device_device_descriptor_add_position(list_get_last(supported_devices), "<time>",
                                          "Set the fridge's delay to <time>");
    device_device_descriptor_add_switch(list_get_last(supported_devices), "state", "Turns on and off the Fridge", true);
    device_device_descriptor_add_position(list_get_last(supported_devices), "on", "Turns on the Fridge");
    device_device_descriptor_add_position(list_get_last(supported_devices), "off", "Turns off the Fridge");
    device_device_descriptor_add_switch(list_get_last(supported_devices), "filling",
                                        "Add or remove items from the Fridge", true);
    device_device_descriptor_add_position(list_get_last(supported_devices), "[-]<N° items>",
                                          "Add or Remove[-] <N° items> from the Fridge");
}

void device_tini(void) {
    free_list(supported_devices);
}

Device *new_device(size_t device_id, size_t device_descriptor_id, const char *name, bool state, void *registry) {
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
    if (name == NULL)
        strncpy(device->name, device->device_descriptor->name, DEVICE_NAME_LENGTH);
    else
        strncpy(device->name, name, DEVICE_NAME_LENGTH);
    device->state = state;
    device->registry = registry;
    device->switches = new_list(NULL, (bool (*)(const void *, const void *)) device_switch_equals);

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
    control_device->devices = new_list(NULL, (bool (*)(const void *, const void *)) device_device_communication_equals);

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
    device_descriptor->switches = new_list(NULL, NULL);

    return device_descriptor;
}

DeviceDescriptorSwitch *new_device_descriptor_switch(char name[], char description[], bool only_manual) {
    DeviceDescriptorSwitch *device_descriptor_switch = (DeviceDescriptorSwitch *) malloc(
            sizeof(DeviceDescriptorSwitch));
    if (device_descriptor_switch == NULL) {
        perror("DeviceDescriptorSwitch Memory Allocation");
        exit(EXIT_FAILURE);
    }

    strncpy(device_descriptor_switch->name, name, DEVICE_SWITCH_NAME_LENGTH);
    strncpy(device_descriptor_switch->description, description, DEVICE_SWITCH_DESCRIPTION_LENGTH);
    device_descriptor_switch->only_manual = only_manual;
    device_descriptor_switch->positions = new_list(NULL, NULL);

    return device_descriptor_switch;
}

DeviceDescriptorSwitchPosition *new_device_descriptor_switch_position(char name[], char description[]) {
    DeviceDescriptorSwitchPosition *device_descriptor_switch_position = (DeviceDescriptorSwitchPosition *) malloc(
            sizeof(DeviceDescriptorSwitchPosition));
    if (device_descriptor_switch_position == NULL) {
        perror("DeviceDescriptorSwitchPosition Memory Allocation");
        exit(EXIT_FAILURE);
    }

    strncpy(device_descriptor_switch_position->name, name, DEVICE_SWITCH_NAME_LENGTH);
    strncpy(device_descriptor_switch_position->description, description, DEVICE_SWITCH_DESCRIPTION_LENGTH);

    return device_descriptor_switch_position;
}

bool device_device_descriptor_add_switch(DeviceDescriptor *device_descriptor, char name[], char description[],
                                         bool only_manual) {
    if (device_descriptor == NULL) return false;

    return list_add_last(device_descriptor->switches, new_device_descriptor_switch(name, description, only_manual));
}

bool device_device_descriptor_add_position(DeviceDescriptor *device_descriptor, char *name,
                                           char *description) {
    DeviceDescriptorSwitch *device_descriptor_switch;
    if (device_descriptor == NULL) return false;
    if (list_is_empty(device_descriptor->switches)) return false;

    device_descriptor_switch = (DeviceDescriptorSwitch *) list_get_last(device_descriptor->switches);
    return list_add_last(device_descriptor_switch->positions, new_device_descriptor_switch_position(name, description));
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

DeviceSwitch *new_device_switch(char name[], void *state, int  (*set_state)(const char *, void *)) {
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

void *device_get_device_switch_state(const List *switch_list, const char *name) {
    return ((DeviceSwitch *) list_get(switch_list, list_get_index(switch_list, name)))->state;
}

DeviceSwitch *device_get_device_switch(const List *switch_list, const char *name) {
    return (DeviceSwitch *) list_get(switch_list, list_get_index(switch_list, name));
}

bool device_check_device(const Device *device) {
    return device != NULL && device->registry != NULL;
}

bool device_check_control_device(const ControlDevice *control_device) {
    return control_device != NULL && device_check_device(control_device->device) && control_device->devices != NULL;
}

bool control_device_fork(const ControlDevice *control_device, size_t id, const DeviceDescriptor *device_descriptor,
                         const char *custom_name) {
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

            control_device_fork_child(id, device_descriptor, custom_name);
            break;
        }
        default: {
            close(write_parent_read_child[0]);
            close(write_child_read_parent[1]);

            list_add_last(control_device->devices,
                          new_device_communication(child_pid, write_child_read_parent[0], write_parent_read_child[1]));

            if (device_communication_read_message(
                    (DeviceCommunication *) list_get_last(control_device->devices)).type != MESSAGE_TYPE_I_AM_ALIVE) {
                list_remove_last(control_device->devices);
                return false;
            }

            break;
        }
    }

    return true;
}

static void
control_device_fork_child(size_t child_id, const DeviceDescriptor *device_descriptor, const char *custom_name) {
    char *device_args[DEVICE_CHILD_ARGS_LENGTH + 1];
    char device_name[DEVICE_NAME_LENGTH];
    char device_id[sizeof(size_t) + 1];
    char device_descriptor_id[sizeof(size_t) + 1];
    if (device_descriptor == NULL) return;

    snprintf(device_name, DEVICE_NAME_LENGTH, "%s", (custom_name != NULL) ? custom_name : device_descriptor->name);
    snprintf(device_id, sizeof(size_t) + 1, "%ld", child_id);
    snprintf(device_descriptor_id, sizeof(size_t) + 1, "%ld", device_descriptor->id);

    device_args[0] = device_name;
    device_args[1] = device_id;
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

static void device_table_print_divider(void) {
    size_t i;
    print("\t");
    for (i = 0; i < DEVICE_DESCRIPTION_LENGTH; ++i) {
        if (i == DEVICE_NAME_LENGTH + 1) print("|");
        else print("-");
    }
    println("");
}

static void device_table_print_left_spacing(const char *string) {
    if (string == NULL)
        print("\t%-*s", DEVICE_NAME_LENGTH, "");
    else
        print("\t%-*s", DEVICE_NAME_LENGTH, string);
}

void device_print_all(void) {
    DeviceDescriptor *data;
    if (supported_devices == NULL) return;

    println_color(COLOR_BOLD, "\t%-*s | %s", DEVICE_NAME_LENGTH, "NAME", "DESCRIPTION");

    list_for_each(data, supported_devices) {
        device_table_print_divider();
        device_print(data);
    }
}

void device_print(const DeviceDescriptor *device_descriptor) {
    DeviceDescriptorSwitch *data;
    DeviceDescriptorSwitchPosition *position;
    const char *color;
    size_t i;
    size_t j;
    if (device_descriptor == NULL) return;

    color = COLOR_WHITE;
    switch (device_descriptor->id) {
        case DEVICE_TYPE_CONTROLLER:
        case DEVICE_TYPE_DOMUS: {
            color = COLOR_CYAN;
            break;
        }
        default: {
            if (device_descriptor->control_device) color = COLOR_YELLOW;
            break;
        }
    }

    print_color(color, "\t%-*s", DEVICE_NAME_LENGTH, device_descriptor->name);
    println(" | %s", device_descriptor->description);

    if (!list_is_empty(device_descriptor->switches)) {
        j = 0;
        list_for_each(data, device_descriptor->switches) {
            color = (data->only_manual) ? COLOR_RED : COLOR_GREEN;

            device_table_print_left_spacing(NULL);
            print(" | ");
            print_color(color, "%s%3s ", COLOR_BOLD, "»");
            println("%-*s %-*s", DEVICE_SWITCH_NAME_LENGTH, data->name, DEVICE_SWITCH_DESCRIPTION_LENGTH,
                    data->description);

            for (i = 0; i < data->positions->size; ++i) {
                position = (DeviceDescriptorSwitchPosition *) list_get(data->positions, i);

                device_table_print_left_spacing(NULL);
                print(" | %4s ", "~");
                println("%-*s %-*s", DEVICE_SWITCH_NAME_LENGTH, position->name, DEVICE_SWITCH_DESCRIPTION_LENGTH,
                        position->description);
            }
            j++;
        }
    }
}

void device_print_legend(void) {
    print("\t");
    print_color(BACKGROUND_COLOR_CYAN, "%s SYSTEM ", COLOR_BOLD);
    print(" ");
    print_color(BACKGROUND_COLOR_YELLOW, "%s CONTROL ", COLOR_BOLD);
    print(" ");
    println_color(COLOR_INVERSE, "%s INTERACTION ", COLOR_BOLD);
}