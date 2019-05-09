
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

                fields = device_communication_split_message_fields(&in_message);
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
        default: {
            device_communication_message_modify(&out_message, MESSAGE_TYPE_ERROR,
                                                "{%d, %s}",
                                                in_message.type,
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

int main(int argc, char **args) {
    hub = device_child_new_control_device(argc, args, new_hub_registry());
    hub_communication = device_child_new_control_device_communication(argc, args, hub_message_handler);

    device_child_run(NULL);

    return EXIT_SUCCESS;
}