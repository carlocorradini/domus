
#include <string.h>
#include <sys/wait.h>
#include "domus.h"
#include "device/device_communication.h"
#include "cli/command/command.h"
#include "util/util_printer.h"
#include "util/util_converter.h"
#include "cli/cli.h"
#include "author.h"

/**
 * Controller that must be defined and it cannot be visible outside this file
 * Only one can exist in the entire program!!!
 */
static ControlDevice *domus = NULL;

/**
 * Initialize all Domus Components
 */
static void domus_init(void);

/**
 * Free all Domus Components
 */
static void domus_tini(void);

/**
 * Propagate a message into the system and return a List of received messages
 *  Remember to free the List using free_list function
 * @param id The id of the recipient Device
 * @param out_message_type The out message type
 * @param out_message_message The out message string message
 * @param in_message_type Incoming message type from Device/s
 * @return The List of received messages, can be empty, NULL otherwise
 */
static List *
domus_propagate_message(size_t id, size_t out_message_type, const char *out_message_message, size_t in_message_type);

void domus_start(void) {
    domus_init();
    cli_start();
    domus_tini();
}

static void domus_init(void) {
    /* Create Domus, only once in the entire program with id 0 */
    domus = new_control_device(
            new_device(DOMUS_ID, DEVICE_TYPE_DOMUS,
                       DEVICE_STATE,
                       new_domus_registry()
            ));
    command_init();
    author_init();
    device_init();
    control_device_fork(domus, CONTROLLER_ID, device_is_supported_by_id(DEVICE_TYPE_CONTROLLER));
}

static void domus_tini(void) {
    free_list(domus_propagate_message(DEVICE_MESSAGE_TO_ALL_DEVICES, MESSAGE_TYPE_TERMINATE, "",
                                      MESSAGE_TYPE_TERMINATE));
    free_list(domus_propagate_message(CONTROLLER_ID, MESSAGE_TYPE_TERMINATE_CONTROLLER, "", MESSAGE_TYPE_TERMINATE));
    free_control_device(domus);
    command_tini();
    author_tini();
    device_tini();
}

DomusRegistry *new_domus_registry(void) {
    DomusRegistry *domus_registry;
    if (device_check_control_device(domus)) return NULL;

    domus_registry = (DomusRegistry *) malloc(sizeof(DomusRegistry));
    if (domus_registry == NULL) {
        perror("Domus Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }

    domus_registry->next_id = CONTROLLER_ID + 1;

    return domus_registry;
}

bool domus_has_devices(void) {
    return control_device_has_devices(domus);
}

size_t domus_fork_device(const DeviceDescriptor *device_descriptor) {
    size_t child_id;
    if (!device_check_control_device(domus) || device_descriptor == NULL) return -1;

    child_id = ((DomusRegistry *) domus->device->registry)->next_id++;
    if (!control_device_fork(domus, child_id, device_descriptor)) return -1;

    return child_id;
}

static List *
domus_propagate_message(size_t id, size_t out_message_type, const char *out_message_message, size_t in_message_type) {
    List *message_list;
    DeviceCommunication *data;
    DeviceCommunicationMessage out_message;
    DeviceCommunicationMessage in_message;
    if (!device_check_control_device(domus)) return false;
    if (!control_device_has_devices(domus)) return false;

    message_list = new_list(NULL, NULL);
    device_communication_message_init(domus->device, &out_message);
    device_communication_message_modify(&out_message, id, out_message_type, out_message_message);
    if (id == DEVICE_MESSAGE_TO_ALL_DEVICES) out_message.flag_force = true;

    list_for_each(data, domus->devices) {
        /* Found a Device with corresponding id */
        if ((in_message = device_communication_write_message_with_ack(data, &out_message)).type == in_message_type) {
            list_add_first(message_list, device_communication_message_copy(&in_message));

            if (in_message.flag_continue) {
                do {
                    if ((in_message = device_communication_write_message_with_ack_silent(data, &out_message)).type ==
                        in_message_type)
                        list_add_first(message_list, device_communication_message_copy(&in_message));
                } while (in_message.flag_continue);
            }

            /* Delete only if type is TERMINATE & is directly connected */
            if (in_message.type == MESSAGE_TYPE_TERMINATE &&
                device_communication_device_is_directly_connected(&in_message)) {
                device_communication_close_communication(data);
                list_remove(domus->devices, data);
            }

            if (id != DEVICE_MESSAGE_TO_ALL_DEVICES) return message_list;
        }
    }

    return message_list;
}

bool domus_del_by_id(size_t id) {
    List *message_list;
    DeviceCommunicationMessage *data;
    DeviceDescriptor *device_descriptor;
    bool toRtn;
    if (!device_check_control_device(domus)) return false;
    if (!control_device_has_devices(domus)) return false;

    message_list = domus_propagate_message(id, MESSAGE_TYPE_TERMINATE, "", MESSAGE_TYPE_TERMINATE);

    list_for_each(data, message_list) {
        device_descriptor = device_is_supported_by_id(data->id_device_descriptor);
        if (device_descriptor == NULL) {
            println_color(COLOR_RED, "Deletion Command: Device with unknown Device Descriptor id %ld",
                          data->id_device_descriptor);
        }

        println_color(COLOR_GREEN,
                      "\t%s with id %ld has been deleted | %ld hop distance",
                      (device_descriptor == NULL) ? "?" : device_descriptor->name,
                      data->id_sender,
                      data->ctr_hop);
    }

    (list_is_empty(message_list)) ? (toRtn = false) : (toRtn = true);

    free_list(message_list);

    return toRtn;
}

bool domus_del_all(void) {
    if (!device_check_control_device(domus)) return false;
    if (!control_device_has_devices(domus)) return false;

    return domus_del_by_id(DEVICE_MESSAGE_TO_ALL_DEVICES);
}

bool domus_info_by_id(size_t id) {
    List *message_list;
    DeviceCommunicationMessage *data;
    DeviceDescriptor *device_descriptor;
    bool toRtn;
    if (!device_check_control_device(domus)) return false;
    if (!control_device_has_devices(domus)) return false;

    message_list = domus_propagate_message(id, MESSAGE_TYPE_INFO, "", MESSAGE_TYPE_INFO);

    list_for_each(data, message_list) {
        device_descriptor = device_is_supported_by_id(data->id_device_descriptor);
        if (device_descriptor == NULL) {
            println_color(COLOR_RED, "Info Command: Device with unknown Device Descriptor id %ld",
                          data->id_device_descriptor);
        }

        char **fields = device_communication_split_message_fields(data);
        ConverterResult device_state = converter_bool_to_string(
                converter_char_to_bool(fields[0][0]).data.Bool);

        print("\tID: %-*ld DEVICE_NAME: %-*s DEVICE_STATE: %-*s",
              sizeof(size_t) + 1, data->id_sender,
              DEVICE_NAME_LENGTH, (device_descriptor == NULL) ? "?" : device_descriptor->name,
              10, device_state.data.String);

        switch (data->id_device_descriptor) {
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
            case DEVICE_TYPE_CONTROLLER: {
                println("DIRECTLY_CONNECTED_CHILD: %-*s",
                        sizeof(double) + 1, fields[1]);
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

        device_communication_free_message_fields(fields);
    }

    (list_is_empty(message_list)) ? (toRtn = false) : (toRtn = true);

    free_list(message_list);

    return toRtn;
}

bool domus_info_all(void) {
    if (!device_check_control_device(domus)) return false;
    if (!control_device_has_devices(domus)) return false;

    return domus_info_by_id(DEVICE_MESSAGE_TO_ALL_DEVICES);
}

void domus_list(void) {
    domus_info_all();
}

int domus_switch(size_t id, const char *switch_label, const char *switch_pos) {
    List *message_list;
    DeviceCommunicationMessage *data;
    DeviceDescriptor *device_descriptor;
    const char out_message_message[DEVICE_COMMUNICATION_MESSAGE_LENGTH];
    int toRtn;
    if (!device_check_control_device(domus)) return false;
    if (!control_device_has_devices(domus)) return false;

    snprintf((char *) out_message_message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "%s\n%s\n", switch_label, switch_pos);
    message_list = domus_propagate_message(id, MESSAGE_TYPE_SET_ON, out_message_message, MESSAGE_TYPE_SET_ON);

    data = (DeviceCommunicationMessage *) list_get_first(message_list);
    device_descriptor = device_is_supported_by_id(data->id_device_descriptor);
    if (device_descriptor == NULL) {
        println_color(COLOR_RED, "Set On Command: Device with unknown Device Descriptor id %ld",
                      data->id_device_descriptor);
    }

    if (strcmp(data->message, MESSAGE_RETURN_SUCCESS) == 0) toRtn = 0;
    else if (strcmp(data->message, MESSAGE_RETURN_NAME_ERROR) == 0) toRtn = 1;
    else if (strcmp(data->message, MESSAGE_RETURN_VALUE_ERROR) == 0) toRtn = 2;
    else toRtn = -1;

    free_list(message_list);

    return toRtn;
}

/**
 * Device Dad Structure
 */
typedef struct DeviceDad {
    size_t id;
    size_t hop_distance;
} DeviceDad;

/**
 * Create a new Device Dad
 * @param id The Id of the Device
 * @param hop_distance The Distance of the Device from this Device
 * @return The New Device Dad, NULL otherwise
 */
static DeviceDad *new_device_dad(size_t id, size_t hop_distance) {
    DeviceDad *device_dad = (DeviceDad *) malloc(sizeof(DeviceDad));
    if (device_dad == NULL) {
        perror("Daddy Memory Allocation");
        exit(EXIT_FAILURE);
    }

    device_dad->id = id;
    device_dad->hop_distance = hop_distance;

    return device_dad;
}

/**
 * Compare two Device Dad
 * @param data_1 first Device Dad
 * @param data_2 second Device Dad
 * @return true if equals, false otherwise
 */
static bool device_dad_equals(const DeviceDad *data_1, const DeviceDad *data_2) {
    if (data_1 == NULL || data_2 == NULL) return false;
    return data_1->hop_distance == data_2->hop_distance;
}

int domus_link(size_t device_id, size_t control_device_id) {
    List *device_list;
    List *device_dad_list;
    DeviceCommunication *data;
    DeviceCommunicationMessage in_message;
    DeviceCommunicationMessage out_message;
    DeviceCommunicationMessage *device_to_spawn;
    DeviceDescriptor *device_descriptor;
    int toRtn;
    if (!device_check_control_device(domus)) return false;

    device_list = domus_propagate_message(device_id, MESSAGE_TYPE_INFO, "", MESSAGE_TYPE_INFO);
    device_dad_list = new_list(NULL, device_dad_equals);
    device_communication_message_init(domus->device, &out_message);
    toRtn = -1;

    /* No Device Found */
    if (list_is_empty(device_list)) toRtn = 1;
    else {
        /* Lock The Device */
        free_list(domus_propagate_message(device_id, MESSAGE_TYPE_LOCK, "", MESSAGE_TYPE_LOCK));

        /* Spawn Devices */
        device_to_spawn = (DeviceCommunicationMessage *) list_get_first(device_list);
        device_communication_message_modify(&out_message, control_device_id, MESSAGE_TYPE_SPAWN_DEVICE,
                                            "%ld\n%ld\n%s",
                                            device_to_spawn->id_sender,
                                            device_to_spawn->id_device_descriptor,
                                            device_to_spawn->message);

        list_for_each(data, domus->devices) {
            switch ((in_message = device_communication_write_message_with_ack(data, &out_message)).type) {
                case MESSAGE_TYPE_ERROR: {
                    /* Something goes wrong, Rollback */
                    free_list(domus_propagate_message(device_id, MESSAGE_TYPE_UNLOCK, "", MESSAGE_TYPE_UNLOCK));
                    device_descriptor = device_is_supported_by_id(in_message.id_device_descriptor);
                    if (device_descriptor == NULL) {
                        println_color(COLOR_RED, "Link Command: Device with unknown Device Descriptor id %ld",
                                      in_message.id_device_descriptor);
                    }

                    println_color(COLOR_RED, "\t%s Error: %s",
                                  (device_descriptor == NULL) ? "?" : device_descriptor->name, in_message.message);
                    toRtn = 3;
                    break;
                }
                case MESSAGE_TYPE_SPAWN_DEVICE: {
                    DeviceDad *device_dad;
                    DeviceDad find_dad;
                    size_t dad_id;

                    list_add_last(device_dad_list,
                                  new_device_dad(device_to_spawn->id_sender, device_to_spawn->ctr_hop));
                    list_remove_first(device_list);

                    while (!list_is_empty(device_list)) {
                        device_to_spawn = (DeviceCommunicationMessage *) list_get_first(device_list);
                        device_dad = new_device_dad(device_to_spawn->id_sender, device_to_spawn->ctr_hop);
                        find_dad.id = device_dad->id;
                        find_dad.hop_distance = device_dad->hop_distance - 1;

                        if (((DeviceDad *) list_get_last(device_dad_list))->hop_distance > device_dad->hop_distance) {
                            while (((DeviceDad *) list_get_last(device_dad_list))->hop_distance >=
                                   device_dad->hop_distance) {
                                list_remove_last(device_dad_list);
                            }
                        }

                        list_add_last(device_dad_list, device_dad);

                        dad_id = ((DeviceDad *) list_get(device_dad_list,
                                                         list_get_index(device_dad_list, &find_dad)))->id;

                        device_communication_message_modify(&out_message, dad_id, MESSAGE_TYPE_SPAWN_DEVICE,
                                                            "%ld\n%ld\n%s",
                                                            device_to_spawn->id_sender,
                                                            device_to_spawn->id_device_descriptor,
                                                            device_to_spawn->message);

                        device_communication_write_message_with_ack(data, &out_message);

                        list_remove_first(device_list);
                    }

                    /* Unlock and delete previous Locked Devices */
                    free_list(domus_propagate_message(device_id, MESSAGE_TYPE_UNLOCK_AND_DELETE, "",
                                                      MESSAGE_TYPE_TERMINATE));
                    toRtn = 0;
                    break;
                }
                default: {
                    break;
                }
            }

            if (toRtn != -1) break;
        }

        /* No Control Device Found, Rollback */
        if (toRtn == -1) {
            toRtn = 2;
            free_list(domus_propagate_message(device_id, MESSAGE_TYPE_UNLOCK, "", MESSAGE_TYPE_UNLOCK));
        }
    }

    free_list(device_list);
    free_list(device_dad_list);

    return toRtn;
}