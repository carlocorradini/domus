
#include <string.h>
#include <sys/wait.h>
#include "device/control/device_controller.h"
#include "device/device_communication.h"
#include "device/device_child.h"
#include "cli/command/command.h"
#include "util/util_printer.h"
#include "util/util_converter.h"
#include "util/util_string_handler.h"
#include "cli/cli.h"
#include "author.h"

/**
 * Controller that must be defined and it cannot be visible outside this file
 * Only one can exist in the entire program!!!
 */
static ControlDevice *controller = NULL;

/**
 * Initialize all Controller Components
 */
static void controller_init(void);

/**
 * Free all Controller Components
 */
static void controller_tini(void);

void controller_start(void) {
    controller_init();
    cli_start();
    controller_tini();
}

static void controller_init(void) {
    /* Create the controller, only once in the entire program with id 0 */
    controller = new_control_device(
            new_device(DEVICE_CONTROLLER_ID, DEVICE_CONTROLLER_ID,
                       DEVICE_STATE,
                       new_controller_registry()
            ));

    command_init();
    author_init();
    device_init();
    device_change_path_file_name(DEVICE_CONTROLLER_DEVICE_PATH);
}

static void controller_tini(void) {
    free_control_device(controller);
    command_tini();
    author_tini();
    device_tini();
}

ControllerRegistry *new_controller_registry(void) {
    ControllerRegistry *controller_registry;
    if (device_check_control_device(controller)) return NULL;

    controller_registry = (ControllerRegistry *) malloc(sizeof(ControllerRegistry));
    if (controller_registry == NULL) {
        perror("Controller Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }

    controller_registry->next_id = 1;

    return controller_registry;
}

bool controller_has_devices(void) {
    return control_device_has_devices(controller);
}

size_t controller_fork_device(const DeviceDescriptor *device_descriptor) {
    size_t child_id;
    if (!device_check_control_device(controller) || device_descriptor == NULL) return -1;

    child_id = ((ControllerRegistry *) controller->device->registry)->next_id++;
    if (!control_device_fork(controller, child_id, device_descriptor)) return -1;

    return child_id;
}

bool controller_del_by_id(size_t id) {
    DeviceCommunication *data;
    DeviceCommunicationMessage out_message;
    DeviceCommunicationMessage in_message;
    const DeviceDescriptor *device_descriptor;
    if (!device_check_control_device(controller)) return false;
    if (!control_device_has_devices(controller)) return false;

    device_communication_message_init(controller->device, &out_message);
    device_communication_message_modify(&out_message, id, MESSAGE_TYPE_TERMINATE, "");
    if (id == DEVICE_CONTROLLER_DELETE_ALL_DEVICES) out_message.type = MESSAGE_TYPE_TERMINATE_FORCED;

    list_for_each(data, controller->devices) {
        /* Found a Device with corresponding id */
        if ((in_message = device_communication_write_message_with_ack(data, &out_message)).type ==
            MESSAGE_TYPE_TERMINATE) {
            device_communication_close_communication(data);
            device_descriptor = device_is_supported_by_id(in_message.id_device_descriptor);
            if (device_descriptor == NULL) {
                fprintf(stderr, "Deletion Command: Device with unknown Device Descriptor id %ld\n",
                        in_message.id_device_descriptor);
            }

            println_color(COLOR_GREEN,
                          "\t%s with id %ld & pid %d has been deleted",
                          (device_descriptor == NULL) ? "?" : device_descriptor->name,
                          in_message.id_sender,
                          data->pid);

            list_remove(controller->devices, data);

            if (id != DEVICE_CONTROLLER_DELETE_ALL_DEVICES) return true;
        }
    }

    if (id == DEVICE_CONTROLLER_DELETE_ALL_DEVICES) return true;
    else return false;
}

bool controller_del_all(void) {
    if (!device_check_control_device(controller)) return false;
    if (!control_device_has_devices(controller)) return false;

    return controller_del_by_id(DEVICE_CONTROLLER_DELETE_ALL_DEVICES);
}

bool controller_info_by_id(size_t id) {
    DeviceCommunication *data;
    DeviceCommunicationMessage out_message;
    DeviceCommunicationMessage in_message;
    const DeviceDescriptor *device_descriptor;
    if (!device_check_control_device(controller)) return false;
    if (!control_device_has_devices(controller)) return false;

    device_communication_message_init(controller->device, &out_message);
    device_communication_message_modify(&out_message, id, MESSAGE_TYPE_INFO, "");
    if (id == DEVICE_CONTROLLER_INFO_ALL_DEVICES) out_message.type = MESSAGE_TYPE_INFO_FORCED;

    println("\t%15s     %15s     %15s     %23s     %15s", "ID", "DEVICE", "STATE", "ACTIVE TIME", "SWITCH STATE");
    list_for_each(data, controller->devices) {
        /* Found a Device with corresponding id */
        if ((in_message = device_communication_write_message_with_ack(data, &out_message)).type ==
            MESSAGE_TYPE_INFO) {
            device_descriptor = device_is_supported_by_id(in_message.id_device_descriptor);
            if (device_descriptor == NULL) {
                fprintf(stderr, "Info Command: Device with unknown Device Descriptor id %ld\n",
                        in_message.id_device_descriptor);
            }

            char **fields = device_communication_split_message(&in_message);

            switch (in_message.id_device_descriptor) {
                case DEVICE_TYPE_BULB: {
                    ConverterResult bulb_state = converter_bool_to_string(
                            converter_char_to_bool(fields[0][0]).data.Bool);
                    ConverterResult bulb_switch_state = converter_bool_to_string(
                            converter_char_to_bool(fields[2][0]).data.Bool);

                    println("\t%15ld     %15s     %15s     %15s seconds     %15s",
                            in_message.id_sender,
                            (device_descriptor == NULL) ? "?" : device_descriptor->name,
                            bulb_state.data.String,
                            fields[1],
                            bulb_switch_state.data.String);
                    break;
                }
            }

            free(fields);

            if (id != DEVICE_CONTROLLER_INFO_ALL_DEVICES) return true;
        }
    }

    if (id == DEVICE_CONTROLLER_INFO_ALL_DEVICES) return true;
    else return false;
}

bool controller_info_all(void) {
    if (!device_check_control_device(controller)) return false;
    if (!control_device_has_devices(controller)) return false;

    return controller_info_by_id(DEVICE_CONTROLLER_INFO_ALL_DEVICES);
}

static void controller_message_handler(DeviceCommunicationMessage in_message) {
    /*ConverterResult result;
    DeviceCommunication *device_communication;

    switch (in_message.type) {
        case MESSAGE_TYPE_ERROR: {
            println_color(COLOR_RED, "\tERROR MESSAGE");
            println("\tMessage from %ld: %s", in_message.id_sender, in_message.message);
            break;
        }
        case MESSAGE_TYPE_INFO: {
            println("\t%s", in_message.message);
            break;
        }
        case MESSAGE_TYPE_TERMINATE: {
            device_communication_close_communication(device_communication);

            result = converter_bool_to_string(converter_char_to_bool(in_message.message[0]).data.Bool);

            println_color(COLOR_GREEN,
                          "\t%s with id %ld & pid %d has been deleted with status %s",
                          device_communication->device_descriptor->name,
                          device_communication->id,
                          device_communication->pid,
                          result.data.String);

            list_remove(controller->devices, device_communication);

            println("\tCLOSED");

            break;
        }
        default: {
            device_communication = control_device_get_device_communication_by_id(in_message.id_sender, controller);
            println_color(COLOR_RED, "\tUNKNOWN MESSAGE");
            println("\tFrom %ld with pid %d: {%d, %s}",
                    device_communication->id,
                    device_communication->pid,
                    in_message.type,
                    in_message.message);
            println("\tUKNOWN MESSAGE");
            break;
        }
    }*/
}

void controller_list(void) {
    DeviceCommunication *data;
    DeviceCommunicationMessage out_message;
    if (!device_check_control_device(controller)) return;

    device_communication_message_init(controller->device, &out_message);
    out_message.type = MESSAGE_TYPE_INFO;

    list_for_each(data, controller->devices) {
        controller_message_handler(device_communication_write_message_with_ack(data, &out_message));
    }
}

int controller_switch(size_t id, const char *switch_label, const char *switch_pos) {
    /*DeviceCommunicationMessage out_message;
    DeviceCommunicationMessage in_message;
    if (!device_check_control_device(controller)) return -1;
    if (!control_device_valid_id(id, controller)) return -1;

    device_communication_message_init(controller->device, &out_message);

    char *a[2];
    a[0] = malloc(MESSAGE_VALUE_LENGTH * sizeof(char));
    strcpy(a[0], switch_label);
    a[1] = malloc(MESSAGE_VALUE_LENGTH * sizeof(char));
    strcpy(a[1], switch_pos);

    device_communication_message_modify(&out_message, MESSAGE_TYPE_SET_ON,
                                        string_array_to_string(a));

    in_message = device_communication_write_message_with_ack(
            control_device_get_device_communication_by_id(id, controller),
            &out_message);

    free(a[0]);
    free(a[1]);

    if (strcmp(in_message.message, MESSAGE_RETURN_SUCCESS) == 0) return 0;
    if (strcmp(in_message.message, MESSAGE_RETURN_NAME_ERROR) == 0) return 1;
    if (strcmp(in_message.message, MESSAGE_RETURN_VALUE_ERROR) == 0) return 2;

    return -1;*/
}

bool controller_link(size_t device_id, size_t control_device_id) {
    /*DeviceCommunication *device_communication;
    DeviceCommunicationMessage out_message;
    DeviceCommunicationMessage in_message;
    if (!device_check_control_device(controller)) return false;
    if (!control_device_valid_id(device_id, controller) || !control_device_valid_id(control_device_id, controller))
        return false;

    device_communication_message_init(controller->device, &out_message);
    out_message.type = MESSAGE_TYPE_CLONE_DEVICE;

    device_communication = control_device_get_device_communication_by_id(device_id, controller);
    in_message = device_communication_write_message_with_ack(device_communication, &out_message);

    println_color(COLOR_RED, "CLONE: %s", in_message.message);
*/
    return true;
}