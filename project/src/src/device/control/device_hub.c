
#include <time.h>
#include <string.h>
#include "device/control/device_hub.h"
#include "device/device_child.h"
#include "util/util_converter.h"

/**
 *  The Hub Control Device
 */
static ControlDevice *hub = NULL;

/**
 * The Device Communication for Hub
 */
static DeviceCommunication *hub_communication = NULL;

/**
 * Handle the incoming message
 * @param in_message The received message
 */
static void hub_message_handler(DeviceCommunicationMessage in_message);

static void hub_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    device_communication_message_init(hub->device, &out_message);

    out_message.override = in_message.override;
    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_INFO,
                                                "%d\n",
                                                hub->device->state);
            break;
        }
        case MESSAGE_TYPE_SET_INIT_VALUES: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SET_INIT_VALUES,
                                                "");
            break;
        }
        case MESSAGE_TYPE_SPAWN_DEVICE: {
            if (!list_is_empty(hub->devices)) {
                char **fields;
                ConverterResult child_descriptor_id;
                DeviceCommunicationMessage child_out_message;
                DeviceCommunicationMessage child_in_message;

                fields = device_communication_split_message_fields(in_message.message);
                child_descriptor_id = converter_string_to_long(fields[1]);
                device_communication_message_init(hub->device, &child_out_message);
                device_communication_message_modify(&child_out_message, hub->device->id, MESSAGE_TYPE_INFO, "");

                child_in_message = device_communication_write_message_with_ack(
                        (DeviceCommunication *) list_get_first(hub->devices), &child_out_message);

                device_communication_free_message_fields(fields);
                if (!child_descriptor_id.error &&
                    child_in_message.id_device_descriptor == child_descriptor_id.data.Long) {
                    device_child_set_device_to_spawn(in_message);
                    return;
                } else {
                    device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_ERROR,
                                                        "Cannot attach a device with different type");
                }
            } else {
                device_child_set_device_to_spawn(in_message);
                return;
            }

            break;
        }
        case MESSAGE_TYPE_SWITCH: {
            char **fields = device_communication_split_message_fields(in_message.message);

            if (fields[2] == NULL) {
                if (strcmp(fields[0], "turn") == 0 || strcmp(fields[0], "state") == 0 ||
                    strcmp(fields[0], "open") == 0) {
                    if (strcmp(fields[1], "on") == 0) {
                        hub->device->state = true;
                    } else if (strcmp(fields[1], "off") == 0) {
                        hub->device->state = false;
                    }
                }
            }
            free(fields);
            break;
        }
        default: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_UNKNOWN, "%s",
                                                in_message.message);
            break;
        }
    }

    device_communication_write_message(hub_communication, &out_message);
}

HubRegistry *new_hub_registry(void) {
    HubRegistry *hub_registry;
    if (hub != NULL) return NULL;

    hub_registry = (HubRegistry *) malloc(sizeof(HubRegistry));
    if (hub_registry == NULL) {
        perror("Hub Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }

    hub_registry->start = time(NULL);

    return hub_registry;
}

static bool
control_device_propagate_message_logic(List *list, DeviceCommunication *device_communication,
                                       const DeviceCommunicationMessage *out_message, size_t in_message_type) {
    DeviceCommunicationMessage in_message;
    if (!device_check_control_device(hub)) return false;
    if (!control_device_has_devices(hub)) return false;
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
    }

    return true;
}

static void queue_message_handler() {
    Message *in_message;
    Queue_message *out_message;
    ConverterResult sender_pid;
    char text[QUEUE_MESSAGE_MESSAGE_LENGTH];
    char **fields;
    int message_id;

    message_id = queue_message_get_message_id(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER);
    in_message = queue_message_receive_message(message_id, QUEUE_MESSAGE_TYPE_DEVICE_START + hub->device->id, true);

    fields = device_communication_split_message_fields(in_message->mesg_text);

    sender_pid = converter_string_to_long(fields[0]);

    DeviceCommunication *data;
    List *message_list;
    DeviceCommunicationMessage device_out_message;

    message_list = new_list(NULL, NULL);
    device_communication_message_init(hub->device, &device_out_message);
    device_communication_message_modify(&device_out_message, -1, MESSAGE_TYPE_SWITCH, "%s\n%s\n", fields[1], fields[2]);
    device_out_message.flag_force = true;
    device_out_message.override = true;

    list_for_each(data, hub->devices) {
        control_device_propagate_message_logic(message_list, data, &device_out_message, MESSAGE_TYPE_SWITCH);
    }

    size_t i;
    bool success = false;

    for (i = 0; i < message_list->size; i++) {
        if (strcmp(((DeviceCommunicationMessage *) list_get(message_list, i))->message, MESSAGE_RETURN_SUCCESS) == 0) {
            success = true;
            break;
        }
    }

    if (success) {
        snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_HUB, MESSAGE_RETURN_SUCCESS);
        if (strcmp(fields[2], "off") == 0) {
            hub->device->state = false;
        }
        if (strcmp(fields[2], "on") == 0) {
            hub->device->state = true;
        }
    } else {
        snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_HUB, QUEUE_MESSAGE_RETURN_NAME_ERROR);
    }
    out_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER,
                                    QUEUE_MESSAGE_TYPE_DEVICE_START + hub->device->id, text, false);
    queue_message_send_message(out_message);
    queue_message_notify((__pid_t) sender_pid.data.Long);

    device_communication_free_message_fields(fields);
}

int main(int argc, char **args) {
    hub = device_child_new_control_device(argc, args, DEVICE_TYPE_HUB, new_hub_registry());
    hub_communication = device_child_new_control_device_communication(argc, args, hub_message_handler);

    signal(DEVICE_COMMUNICATION_READ_QUEUE, queue_message_handler);
    device_child_run(NULL);

    return EXIT_SUCCESS;
}