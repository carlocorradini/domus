
#include <string.h>
#include <sys/wait.h>
#include "device/control/device_controller.h"
#include "device/device_communication.h"
#include "device/device_child.h"
#include "cli/command/command.h"
#include "util/util_printer.h"
#include "util/util_converter.h"
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

/**
 *
 * @param id The recipient id, -1 to send to all devices in the system
 * @param message_type The message type to send
 * @param _controller_command_handler Handle incoming message
 * @return true if send and propagate, false otherwise
 */
static bool controller_propagate_command_message(size_t id, size_t message_type,
                                                 void (*_controller_command_handler)(
                                                         const DeviceCommunicationMessage *));

static void _controller_del_by_id(const DeviceCommunicationMessage *in_message);

static void _controller_info_by_id(const DeviceCommunicationMessage *in_message);

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

static bool controller_propagate_command_message(size_t id, size_t message_type,
                                                 void (*_controller_command_handler)(
                                                         const DeviceCommunicationMessage *)) {
    DeviceCommunication *data;
    DeviceCommunicationMessage out_message;
    DeviceCommunicationMessage in_message;
    if (!device_check_control_device(controller)) return false;
    if (!control_device_has_devices(controller)) return false;

    device_communication_message_init(controller->device, &out_message);
    device_communication_message_modify(&out_message, id, message_type, "");
    if (id == -1) out_message.flag_force = true;

    list_for_each(data, controller->devices) {
        /* Found a Device with corresponding id */
        if ((in_message = device_communication_write_message_with_ack(data, &out_message)).type == message_type) {
            _controller_command_handler(&in_message);

            if (in_message.flag_continue) {
                do {
                    in_message = device_communication_write_message_with_ack_silent(data, &out_message);
                    _controller_command_handler(&in_message);
                } while (in_message.flag_continue);
            }

            /* Delete only if type is TERMINATE & is directly connected */
            if (message_type == MESSAGE_TYPE_TERMINATE &&
                device_communication_device_is_directly_connected(&in_message)) {
                device_communication_close_communication(data);
                list_remove(controller->devices, data);
            }

            if (id != -1) return true;
        }
    }

    if (id == -1) return true;
    else return false;
}

bool controller_del_by_id(size_t id) {
    return controller_propagate_command_message(id, MESSAGE_TYPE_TERMINATE, _controller_del_by_id);
}

bool controller_del_all(void) {
    if (!device_check_control_device(controller)) return false;
    if (!control_device_has_devices(controller)) return false;

    return controller_del_by_id(DEVICE_CONTROLLER_TO_ALL_DEVICES);
}

bool controller_info_by_id(size_t id) {
    return controller_propagate_command_message(id, MESSAGE_TYPE_INFO, _controller_info_by_id);
}

bool controller_info_all(void) {
    if (!device_check_control_device(controller)) return false;
    if (!control_device_has_devices(controller)) return false;

    return controller_info_by_id(DEVICE_CONTROLLER_TO_ALL_DEVICES);
}

static void _controller_del_by_id(const DeviceCommunicationMessage *in_message) {
    const DeviceDescriptor *device_descriptor;
    if (in_message == NULL) return;

    device_descriptor = device_is_supported_by_id(in_message->id_device_descriptor);
    if (device_descriptor == NULL) {
        fprintf(stderr, "Deletion Command: Device with unknown Device Descriptor id %ld\n",
                in_message->id_device_descriptor);
    }

    println_color(COLOR_GREEN,
                  "\t%s with id %ld has been deleted | %ld hop distance",
                  (device_descriptor == NULL) ? "?" : device_descriptor->name,
                  in_message->id_sender,
                  in_message->ctr_hop);
}

static void _controller_info_by_id(const DeviceCommunicationMessage *in_message) {
    const DeviceDescriptor *device_descriptor;
    if (in_message == NULL) return;

    device_descriptor = device_is_supported_by_id(in_message->id_device_descriptor);
    if (device_descriptor == NULL) {
        fprintf(stderr, "Info Command: Device with unknown Device Descriptor id %ld\n",
                in_message->id_device_descriptor);
    }

    char **fields = device_communication_split_message_fields(in_message);
    ConverterResult device_state = converter_bool_to_string(
            converter_char_to_bool(fields[0][0]).data.Bool);

    print("\tID: %-*ld DEVICE_NAME: %-*s DEVICE_STATE: %-*s",
          sizeof(size_t) + 1, in_message->id_sender,
          DEVICE_NAME_LENGTH, (device_descriptor == NULL) ? "?" : device_descriptor->name,
          10, device_state.data.String);

    switch (in_message->id_device_descriptor) {
        case DEVICE_TYPE_BULB: {
            ConverterResult bulb_switch_state = converter_bool_to_string(
                    converter_char_to_bool(fields[2][0]).data.Bool);

            println("\tACTIVE_TIME(s): %-*s SWITCH_STATE: %-*s",
                    sizeof(double) + 1, fields[1],
                    DEVICE_SWITCH_NAME_LENGTH, bulb_switch_state.data.String);
            break;
        }
        case DEVICE_TYPE_WINDOW : {
            ConverterResult window_switch_state = converter_bool_to_string(
                    converter_char_to_bool(fields[2][0]).data.Bool);

            println("\tOPEN_TIME(s): %-*s   SWITCH_STATE: %-*s",
                    sizeof(double) + 1, fields[1],
                    DEVICE_SWITCH_NAME_LENGTH, window_switch_state.data.String);
            break;
        }
        case DEVICE_TYPE_FRIDGE: {
            ConverterResult fridge_door_switch_state = converter_bool_to_string(
                    converter_char_to_bool(fields[6][0]).data.Bool);

            println("\tOPEN_TIME(s): %-*s   DELAY_TIME(s): %-*s     PERC(%): %-*s     TEMP(C°): %-*s     SWITCH_THERMO: %-*s     SWITCH_DOOR: %-*s",
                    sizeof(double) + 1, fields[1],
                    sizeof(double) + 1, fields[2],
                    sizeof(double) + 1, fields[3],
                    sizeof(double) + 1, fields[4],
                    sizeof(double) + 1, fields[5],
                    DEVICE_SWITCH_NAME_LENGTH, fridge_door_switch_state.data.String);
            break;
        }
        case DEVICE_TYPE_HUB: {
            println("");
            break;
        }
        case DEVICE_TYPE_TIMER: {
            println("");
            break;
        }
        default: {
            println("");
            break;
        }
    }

    free(fields);
}

void controller_list(void) {
}

int controller_switch(size_t id, const char *switch_label, const char *switch_pos) {
    DeviceCommunication *data;
    DeviceCommunicationMessage out_message;
    DeviceCommunicationMessage in_message;
    const DeviceDescriptor *device_descriptor;
    if (!device_check_control_device(controller)) return false;
    if (!control_device_has_devices(controller)) return false;


    device_communication_message_init(controller->device, &out_message);

    /*
    char buffer[26];
    char buffer1[26];
    time_t start = time(NULL) + 15;
    time_t end = time(NULL) + 30;

    struct tm * as = localtime(&start);
    strftime(buffer, 26, "%F-%T" , as);

    as = localtime(&end);
    strftime(buffer1, 26, "%F-%T" , as);
    fprintf(stderr, "%s\n%s\n", buffer, buffer1);
    */
    device_communication_message_modify(&out_message, id, MESSAGE_TYPE_SET_ON, "%s\n%s\n", switch_label, switch_pos);

    /*
    device_communication_message_modify(&out_message, id, MESSAGE_TYPE_SET_ON,
                                        "time\n%s?%s\n", buffer, buffer1);
*/

    list_for_each(data, controller->devices) {

        if ((in_message = device_communication_write_message_with_ack(data, &out_message)).type ==
            MESSAGE_TYPE_SET_ON) {
            device_descriptor = device_is_supported_by_id(in_message.id_device_descriptor);
            if (device_descriptor == NULL) {
                fprintf(stderr, "Set On Command: Device with unknown Device Descriptor id %ld\n",
                        in_message.id_device_descriptor);
            }
        }
    }

    if (strcmp(in_message.message, MESSAGE_RETURN_SUCCESS) == 0) return 0;
    if (strcmp(in_message.message, MESSAGE_RETURN_NAME_ERROR) == 0) return 1;
    if (strcmp(in_message.message, MESSAGE_RETURN_VALUE_ERROR) == 0) return 2;

    return -1;
    /**/
}

typedef struct dad_device {
    size_t id;
    size_t hop_distance;
} dad_device;

static bool dad_device_equals(const dad_device *data_1, const dad_device *data_2) {
    if (data_1 == NULL || data_2 == NULL) return false;
    return data_1->hop_distance == data_2->hop_distance;
}

bool controller_link(size_t device_id, size_t control_device_id) {
    List *device_list;
    DeviceCommunication *data;
    DeviceCommunicationMessage out_message;
    DeviceCommunicationMessage in_message;
    DeviceCommunicationMessage child_out_message;
    if (!device_check_control_device(controller)) return false;

    device_list = new_list(NULL, NULL);
    device_communication_message_init(controller->device, &out_message);
    device_communication_message_init(controller->device, &child_out_message);
    device_communication_message_modify(&out_message, device_id, MESSAGE_TYPE_INFO, "%d", MESSAGE_TYPE_CLONE);

    list_for_each(data, controller->devices) {
        if ((in_message = device_communication_write_message_with_ack(data, &out_message)).type ==
            MESSAGE_TYPE_INFO) {
            list_add_first(device_list, device_communication_message_copy(&in_message));

            if (in_message.flag_continue) {
                do {
                    in_message = device_communication_write_message_with_ack_silent(data, &out_message);
                    list_add_first(device_list, device_communication_message_copy(&in_message));
                } while (in_message.flag_continue);
            }

            break;
        }
    }

    if (list_is_empty(device_list)) return false;

    size_t i;
    for (i = 0; i < device_list->size; i++) {
        controller_del_by_id(((DeviceCommunicationMessage *) list_get(device_list, i))->id_sender);
    }

    DeviceCommunicationMessage *first_device = (DeviceCommunicationMessage *) list_get_first(device_list);
    device_communication_message_modify(&out_message, control_device_id, MESSAGE_TYPE_SPAWN_DEVICE,
                                        "%ld\n%ld\n%s",
                                        first_device->id_sender,
                                        first_device->id_device_descriptor,
                                        first_device->message);

    List *dad_id_list = new_list(NULL, dad_device_equals);
    list_for_each(data, controller->devices) {
        if (device_communication_write_message_with_ack(data, &out_message).type ==
            MESSAGE_TYPE_SPAWN_DEVICE) {
            dad_device *daddy = (dad_device *) malloc(sizeof(dad_device));
            daddy->id = first_device->id_sender;
            daddy->hop_distance = first_device->ctr_hop;
            list_add_last(dad_id_list, daddy);

            list_remove_first(device_list);

            while (!list_is_empty(device_list)) {
                size_t dad_id;
                first_device = (DeviceCommunicationMessage *) list_get_first(device_list);

                daddy = (dad_device *) malloc(sizeof(dad_device));
                daddy->id = first_device->id_sender;
                daddy->hop_distance = first_device->ctr_hop;

                if (((dad_device *) list_get_last(dad_id_list))->hop_distance > daddy->hop_distance) {
                    while (((dad_device *) list_get_last(dad_id_list))->hop_distance >= daddy->hop_distance) {
                        list_remove_last(dad_id_list);
                    }
                }

                if (!list_contains(dad_id_list, &daddy)) {
                    list_add_last(dad_id_list, daddy);
                }

                dad_device find_daddy;
                find_daddy.id = daddy->id;
                find_daddy.hop_distance = daddy->hop_distance - 1;


                dad_id = ((dad_device *) list_get(dad_id_list, list_get_index(dad_id_list, &find_daddy)))->id;

                device_communication_message_modify(&out_message, dad_id, MESSAGE_TYPE_SPAWN_DEVICE,
                                                    "%ld\n%ld\n%s",
                                                    first_device->id_sender,
                                                    first_device->id_device_descriptor,
                                                    first_device->message);

                device_communication_write_message_with_ack(data, &out_message);

                list_remove_first(device_list);
            }

            break;
        }
    }

    return true;
}