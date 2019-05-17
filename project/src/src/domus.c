
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

/**
 * Propagate a message into the system and populate the List of received messages
 * @param list The list to populate
 * @param device_communication The Device Communication
 * @param out_message The out message
 * @param in_message_type The incoming message type
 * @return true if propagate goes well, false otherwise
 */
static bool domus_propagate_message_logic(List *list, DeviceCommunication *device_communication,
                                          const DeviceCommunicationMessage *out_message, size_t in_message_type);

/**
 * The queue_message_handler, it handles the incoming
 * queue messages and send them back
 */
static void queue_message_handler();

void domus_start(void) {
    domus_init();
    cli_start();
    domus_tini();
}

static void domus_init(void) {
    /* Create Domus, only once in the entire program with id 0 */
    domus = new_control_device(
            new_device(DOMUS_ID, DEVICE_TYPE_DOMUS, NULL,
                       DEVICE_STATE,
                       new_domus_registry()
            ));
    command_init();
    author_init();
    device_init();
    signal(DEVICE_COMMUNICATION_READ_QUEUE, queue_message_handler);
    control_device_fork(domus, CONTROLLER_ID, device_is_supported_by_id(DEVICE_TYPE_CONTROLLER), NULL);
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

size_t domus_fork_device(const DeviceDescriptor *device_descriptor, const char *custom_name) {
    size_t child_id;
    if (!device_check_control_device(domus) || device_descriptor == NULL) return -1;

    child_id = ((DomusRegistry *) domus->device->registry)->next_id++;
    if (!control_device_fork(domus, child_id, device_descriptor, custom_name)) return -1;

    return child_id;
}

static List *
domus_propagate_message(size_t id, size_t out_message_type, const char *out_message_message, size_t in_message_type) {
    List *message_list;
    DeviceCommunication *data;
    DeviceCommunicationMessage out_message;
    if (!device_check_control_device(domus)) return NULL;
    if (!control_device_has_devices(domus)) return NULL;

    message_list = new_list(NULL, NULL);
    device_communication_message_init(domus->device, &out_message);
    device_communication_message_modify(&out_message, id, out_message_type, out_message_message);
    if (id == DEVICE_MESSAGE_TO_ALL_DEVICES) out_message.flag_force = true;

    if (out_message_type == MESSAGE_TYPE_SWITCH) {
        data = (DeviceCommunication *) list_get_first(domus->devices);
        domus_propagate_message_logic(message_list, data, &out_message, in_message_type);
    } else {
        list_for_each(data, domus->devices) {
            domus_propagate_message_logic(message_list, data, &out_message, in_message_type);
            if (message_list->size > 0 && id != DEVICE_MESSAGE_TO_ALL_DEVICES) return message_list;
        }
    }

    return message_list;
}

static bool domus_propagate_message_logic(List *list, DeviceCommunication *device_communication,
                                          const DeviceCommunicationMessage *out_message, size_t in_message_type) {
    DeviceCommunicationMessage in_message;
    if (!device_check_control_device(domus)) return false;
    if (!control_device_has_devices(domus)) return false;
    if (list == NULL || device_communication == NULL || out_message == NULL) return false;


    if ((in_message = device_communication_write_message_with_ack(device_communication, out_message)).type ==
        in_message_type) {
        list_add_first(list, device_communication_message_copy(&in_message));

        if (in_message.flag_continue) {
            do {
                in_message = device_communication_write_message_with_ack_silent(device_communication, out_message);
                list_add_first(list, device_communication_message_copy(&in_message));
            } while (in_message.flag_continue);
        }

        /* Delete only if type is TERMINATE & is directly connected */
        if (in_message.type == MESSAGE_TYPE_TERMINATE &&
            device_communication_device_is_directly_connected(&in_message)) {
            device_communication_close_communication(device_communication);
            list_remove(domus->devices, device_communication);
        }
    }

    return true;
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
        if (data->type == MESSAGE_TYPE_TERMINATE) {
            device_descriptor = device_is_supported_by_id(data->id_device_descriptor);
            if (device_descriptor == NULL) {
                println_color(COLOR_RED, "\tDeletion Command: Device with unknown Device Descriptor id %ld",
                              data->id_device_descriptor);
            }

            println_color(COLOR_GREEN,
                          "\t%s with id %ld has been deleted",
                          (device_descriptor == NULL) ? "?" : device_descriptor->name,
                          data->id_sender);
        }
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

static void device_table_print_divider(void) {
    size_t i;
    print("\t");
    for (i = 0; i < (sizeof(size_t) + 1 + DEVICE_NAME_LENGTH + DEVICE_STATE_LENGTH + (6 * DEVICE_STATE_LENGTH)); ++i) {
        if (i == sizeof(size_t) + 2 ||
            i == sizeof(size_t) + DEVICE_NAME_LENGTH + 5 ||
            i == sizeof(size_t) + DEVICE_NAME_LENGTH + DEVICE_STATE_LENGTH + 8)
            print("|");
        else print("-");
    }
    println("");
}

bool domus_info_by_id(size_t id) {
    List *message_list;
    DeviceCommunicationMessage *data;
    DeviceDescriptor *device_descriptor;
    char **fields;
    bool device_state;
    const char *color;
    bool toRtn;
    if (!device_check_control_device(domus)) return false;
    if (!control_device_has_devices(domus)) return false;

    message_list = domus_propagate_message(id, MESSAGE_TYPE_INFO, "", MESSAGE_TYPE_INFO);

    if (!list_is_empty(message_list)) {
        device_print_legend();
        println("");
        println_color(COLOR_BOLD, "\t%-*s | %-*s | %-*s | %-*s | %-*s | ",
                      sizeof(size_t) + 1, "ID",
                      DEVICE_NAME_LENGTH, "TYPE",
                      DEVICE_NAME_LENGTH, "NAME",
                      DEVICE_STATE_LENGTH, "OVERRIDE",
                      DEVICE_STATE_LENGTH, "STATE");
    }

    list_for_each(data, message_list) {
        device_table_print_divider();
        device_descriptor = device_is_supported_by_id(data->id_device_descriptor);
        if (device_descriptor == NULL) {
            println_color(COLOR_RED, "\tInfo Command: Device with unknown Device Descriptor id %ld",
                          data->id_device_descriptor);
        }

        fields = device_communication_split_message_fields(data->message);
        device_state = converter_char_to_bool(fields[0][0]).data.Bool;
        color = COLOR_WHITE;

        if (device_descriptor != NULL) {
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
        }

        print("\t%-*ld | ",
              sizeof(size_t) + 1, data->id_sender);
        print_color(color, "%-*s", DEVICE_NAME_LENGTH, (device_descriptor == NULL) ? "?" : device_descriptor->name);
        print(" | %-*s | %-*s | ", DEVICE_NAME_LENGTH, data->device_name, DEVICE_STATE_LENGTH,
              (data->override) ? "yes" : "no");

        switch (data->id_device_descriptor) {
            case DEVICE_TYPE_BULB: {
                bool bulb_switch_state = converter_char_to_bool(fields[2][0]).data.Bool;

                println("%-*s | %-*s: %-*s | %-*s: %s",
                        DEVICE_STATE_LENGTH, (device_state) ? "on" : "off",
                        DEVICE_STATE_LENGTH, "ACTIVE_TIME(s)",
                        sizeof(double) + 1, fields[1],
                        DEVICE_STATE_LENGTH, "SWITCH_TURN",
                        (bulb_switch_state) ? "on" : "off");
                break;
            }
            case DEVICE_TYPE_WINDOW : {
                bool window_switch_state = converter_char_to_bool(fields[2][0]).data.Bool;

                println("%-*s | %-*s: %-*s | %-*s: %s",
                        DEVICE_STATE_LENGTH, (device_state) ? "open" : "close",
                        DEVICE_STATE_LENGTH, "OPEN_TIME(s)",
                        sizeof(double) + 1, fields[1],
                        DEVICE_STATE_LENGTH, "SWITCH_OPEN",
                        (window_switch_state) ? "on" : "off");
                break;
            }
            case DEVICE_TYPE_FRIDGE: {
                bool fridge_door_switch_state = converter_char_to_bool(fields[5][0]).data.Bool;

                println("%-*s | %-*s: %-*s | %-*s: %-*s | %-*s: %-*s | %-*s: %-*s | %-*s: %s",
                        DEVICE_STATE_LENGTH, (fridge_door_switch_state) ? "open" : "close",
                        DEVICE_STATE_LENGTH, "SWITCH_STATE",
                        sizeof(double) + 1, (device_state) ? "on" : "off",
                        DEVICE_STATE_LENGTH, "OPEN_TIME(s)",
                        sizeof(double) + 1, fields[1],
                        DEVICE_STATE_LENGTH, "DELAY_TIME(s)",
                        sizeof(double) + 1, fields[2],
                        DEVICE_STATE_LENGTH, "FILLING(%)",
                        sizeof(double) + 1, fields[3],
                        DEVICE_STATE_LENGTH, "TEMP(C°)",
                        fields[4]);
                break;
            }
            case DEVICE_TYPE_CONTROLLER: {
                println("%-*s | %-*s: %s",
                        DEVICE_STATE_LENGTH, (device_state) ? "on" : "off",
                        DEVICE_STATE_LENGTH, "DIR_CONN_DEV",
                        fields[1]);
                break;
            }
            case DEVICE_TYPE_HUB: {
                println("%-*s |",
                        DEVICE_STATE_LENGTH, (device_state) ? "on" : "off");
                break;
            }
            case DEVICE_TYPE_TIMER: {
                println("%-*s | %-*s: %-*s | %-*s: %-*s",
                        DEVICE_STATE_LENGTH, (device_state) ? "on" : "off",
                        DEVICE_STATE_LENGTH, "START_TIME",
                        sizeof(double) + 1, fields[1],
                        DEVICE_STATE_LENGTH, "END_TIME",
                        sizeof(double) + 1, fields[2]);
                break;
            }
            default: {
                println_color(COLOR_RED, "Unknown Device");
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

void domus_switch(size_t id, const char *switch_label, const char *switch_pos) {
    List *message_list;
    DeviceCommunicationMessage *data;
    DeviceDescriptor *device_descriptor;
    const char out_message_message[DEVICE_COMMUNICATION_MESSAGE_LENGTH];
    const char controller_name[DEVICE_NAME_LENGTH];
    if (!device_check_control_device(domus)) return;
    if (!control_device_has_devices(domus)) return;

    snprintf((char *) out_message_message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "%s\n%s\n", switch_label, switch_pos);
    strncpy((char *) controller_name, device_is_supported_by_id(DEVICE_TYPE_CONTROLLER)->name, DEVICE_NAME_LENGTH);
    message_list = domus_propagate_message(id, MESSAGE_TYPE_SWITCH, out_message_message, MESSAGE_TYPE_SWITCH);

    if (list_is_empty(message_list)) {
        /* No Device under controller */
        message_list = domus_propagate_message(id, MESSAGE_TYPE_INFO, "", MESSAGE_TYPE_INFO);
        if (list_is_empty(message_list)) {
            /* No Device in the entire System */
            println("\tCannot find a Device with id %ld", id);
        } else {
            /* Device found but is not linked to the controller */
            list_for_each(data, message_list) {
                if (data->id_sender == id) {
                    device_descriptor = device_is_supported_by_id(data->id_device_descriptor);
                    if (device_descriptor == NULL) {
                        println_color(COLOR_RED, "\tSwitch Command: Device with unknown Device Descriptor id %ld",
                                      data->id_device_descriptor);
                    }
                    println("\tDevice %s has been found but is NOT linked to %s",
                            (device_descriptor == NULL) ? "?" : device_descriptor->name, controller_name);
                    println("\tPlease link %s with id %ld to the %s with id %ld",
                            (device_descriptor == NULL) ? "?" : device_descriptor->name, id, controller_name,
                            CONTROLLER_ID);
                    println("\tTry type:");
                    println_color(COLOR_YELLOW, "\t\tlink %ld to %ld", id, CONTROLLER_ID);
                }
            }
        }
    } else {
        list_for_each(data, message_list) {
            if (data->type == MESSAGE_TYPE_SWITCH) {
                device_descriptor = device_is_supported_by_id(data->id_device_descriptor);
                if (device_descriptor == NULL) {
                    println_color(COLOR_RED, "\tSet On Command: Device with unknown Device Descriptor id %ld",
                                  data->id_device_descriptor);
                }
                print("\t[%3ld] %-*s ", data->id_sender, DEVICE_NAME_LENGTH,
                      (device_descriptor == NULL) ? "?" : device_descriptor->name);

                if (strcmp(data->message, MESSAGE_RETURN_SUCCESS) == 0) {
                    print_color(COLOR_GREEN, "Switched ");
                    print("'%s'", switch_label);
                    print_color(COLOR_GREEN, " to ");
                    println("'%s'", switch_pos);
                } else if (strcmp(data->message, MESSAGE_RETURN_NAME_ERROR) == 0) {
                    println_color(COLOR_RED, "<label> %s doesn't exist",
                                  switch_label);
                } else if (strcmp(data->message, MESSAGE_RETURN_VALUE_ERROR) == 0) {
                    println_color(COLOR_RED, "<pos> %s doesn't exist",
                                  switch_pos);
                } else if (strcmp(data->message, MESSAGE_RETURN_VALUE_PASSED_DATE_ERROR) == 0) {
                    println_color(COLOR_RED, "The inserted date has already passed");
                } else if (strcmp(data->message, MESSAGE_RETURN_VALUE_ORDER_DATE_ERROR) == 0) {
                    println_color(COLOR_RED, "Please insert the dates in the right order");
                } else if (strcmp(data->message, MESSAGE_RETURN_VALUE_FORMAT_DATE_ERROR) == 0) {
                    println_color(COLOR_RED, "Date format not valid");
                } else if (strcmp(data->message, MESSAGE_RETURN_VALUE_ALREADY_DEFINED_DATE_ERROR) == 0) {
                    println_color(COLOR_RED, "Timer values already defined");
                } else if (strcmp(data->message, MESSAGE_RETURN_VALUE_SAME_DATE_ERROR) == 0) {
                    println_color(COLOR_RED, "The two dates should be different");
                } else if (strcmp(data->message, MESSAGE_RETURN_VALUE_EXCEEDED_FRIDGE_ERROR) == 0) {
                    println_color(COLOR_RED, "Maximum fridge capacity reached");
                } else if (strcmp(data->message, MESSAGE_RETURN_VALUE_EMPTY_FRIDGE_ERROR) == 0) {
                    println_color(COLOR_RED, "Fridge is empty");
                } else if (strcmp(data->message, MESSAGE_RETURN_VALUE_MAXTHERMO_FRIDGE_ERROR) == 0) {
                    println_color(COLOR_RED, "Cannot set internal temperature : too high");
                } else if (strcmp(data->message, MESSAGE_RETURN_VALUE_MINTHERMO_FRIDGE_ERROR) == 0) {
                    println_color(COLOR_RED, "Cannot set internal temperature : too low");
                } else {
                    println_color(COLOR_RED, "Unknown Error");
                }
            }
        }
    }

    free_list(message_list);
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
    if (!device_check_control_device(domus)) return -1;
    if (device_id == control_device_id) return -1;

    device_list = domus_propagate_message(device_id, MESSAGE_TYPE_INFO, "", MESSAGE_TYPE_INFO);
    device_dad_list = new_list(NULL, (bool (*)(const void *, const void *)) device_dad_equals);
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
        strncpy(out_message.device_name, device_to_spawn->device_name, DEVICE_NAME_LENGTH);

        list_for_each(data, domus->devices) {
            switch ((in_message = device_communication_write_message_with_ack(data, &out_message)).type) {
                case MESSAGE_TYPE_ERROR: {
                    /* Something goes wrong, Rollback */
                    free_list(domus_propagate_message(device_id, MESSAGE_TYPE_UNLOCK, "", MESSAGE_TYPE_UNLOCK));
                    device_descriptor = device_is_supported_by_id(in_message.id_device_descriptor);
                    if (device_descriptor == NULL) {
                        println_color(COLOR_RED, "\tLink Command: Device with unknown Device Descriptor id %ld",
                                      in_message.id_device_descriptor);
                    }

                    println_color(COLOR_RED, "\t%s Error: %s",
                                  (device_descriptor == NULL) ? "?" : device_descriptor->name,
                                  in_message.message);
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

                        if (((DeviceDad *) list_get_last(device_dad_list))->hop_distance >
                            device_dad->hop_distance) {
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
                        strncpy(out_message.device_name, device_to_spawn->device_name, DEVICE_NAME_LENGTH);

                        device_communication_write_message_with_ack(data, &out_message);

                        list_remove_first(device_list);
                    }

                    /* Unlock and delete previous Locked Devices */
                    free_list(domus_propagate_message(device_id, MESSAGE_TYPE_UNLOCK_AND_TERMINATE, "",
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

void domus_hierarchy(void) {
    List *device_list;
    DeviceCommunicationMessage *data;
    DeviceDescriptor *device_descriptor;
    size_t i;
    const char *color;
    int old_hop = 0;

    if (!device_check_control_device(domus)) return;

    device_list = domus_propagate_message(DEVICE_MESSAGE_TO_ALL_DEVICES, MESSAGE_TYPE_INFO, "",
                                          MESSAGE_TYPE_INFO);
    println_color(COLOR_CYAN, "\tDOMUS");

    list_for_each(data, device_list) {
        color = COLOR_WHITE;
        device_descriptor = device_is_supported_by_id(data->id_device_descriptor);
        if (device_descriptor == NULL) {
            println_color(COLOR_RED, "\tHierarchy Command: Device with unknown Device Descriptor id %ld",
                          data->id_device_descriptor);
        }

        if (device_descriptor != NULL) {
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
        }

        if (old_hop != 0) {
            print("\t");
            for (i = 0; i < old_hop; i++) print("   ");
            print("\033[3D");
            print("\033[1A");
            print("%s", (old_hop - data->ctr_hop == 0) ? "├─" : "└─");
            print("\033[1B");
            print("\033[100D");
        }
        old_hop = data->ctr_hop;
        print("\t");
        for (i = 0; i < data->ctr_hop; i++) print("   ");

        if (strcmp(device_descriptor->name, data->device_name) == 0) {
            println_color(color, "%s %ld", device_descriptor->name, data->id_sender);
        } else {
            println_color(color, "%s %s %ld", device_descriptor->name, data->device_name, data->id_sender);
        }
    }

    print("\t");
    for (i = 0; i < old_hop; i++) print("   ");
    print("\033[3D");
    print("\033[1A");
    print("└─");
    print("\033[1B");
    print("\033[100D");

    free_list(device_list);
}

pid_t domus_getpid(size_t device_id) {
    List *message_list;
    ConverterResult pid;
    bool toRtn = false;

    if (!device_check_control_device(domus)) return false;
    if (!control_device_has_devices(domus)) return false;

    if (device_id == DOMUS_ID) {
        return getpid();
    }
    message_list = domus_propagate_message(device_id, MESSAGE_TYPE_GET_PID, "", MESSAGE_TYPE_GET_PID);

    if (!list_is_empty(message_list)) {
        pid = converter_string_to_long(((DeviceCommunicationMessage *) list_get_first(message_list))->message);
        if (!pid.error) {
            toRtn = true;
        }
    }
    free_list(message_list);

    return (toRtn) ? (pid_t) pid.data.Long : (pid_t) 0;
}

static void queue_message_handler() {
    ConverterResult result;
    Queue_message *out_message;
    Message *in_message;
    int message_id;
    if (!device_check_control_device(domus)) return;
    if (!control_device_has_devices(domus)) return;

    message_id = queue_message_get_message_id(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER);
    in_message = queue_message_receive_message(message_id, QUEUE_MESSAGE_TYPE_ALL_TYPES, true);

    switch (in_message->mesg_type) {
        case QUEUE_MESSAGE_TYPE_DOMUS_PID_REQUEST : {

            result = converter_string_to_long(in_message->mesg_text);

            char text[QUEUE_MESSAGE_MESSAGE_LENGTH];
            snprintf(text, 64, "%d", getpid());

            out_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER,
                                            QUEUE_MESSAGE_TYPE_DOMUS_PID_REQUEST, text, false);
            queue_message_send_message(out_message);
            queue_message_notify((__pid_t) result.data.Long);

            break;
        }

        case QUEUE_MESSAGE_TYPE_PID_REQUEST: {
            DeviceCommunicationMessage *fake_message;
            char text[QUEUE_MESSAGE_MESSAGE_LENGTH];
            char **fields;
            __pid_t device_pid;

            fake_message = malloc(sizeof(DeviceCommunicationMessage));
            device_communication_message_modify_message(fake_message, in_message->mesg_text);

            fields = device_communication_split_message_fields(fake_message->message);

            result = converter_string_to_long(fields[0]);

            device_pid = domus_getpid(result.data.Long);

            snprintf(text, 64, "%d", device_pid);

            result = converter_string_to_long(fields[1]);

            out_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER,
                                            QUEUE_MESSAGE_TYPE_PID_REQUEST, text, false);
            queue_message_send_message(out_message);
            queue_message_notify((__pid_t) result.data.Long);

            free(fake_message);
            device_communication_free_message_fields(fields);
            break;
        }
        default: {
            break;
        }
    }
}
