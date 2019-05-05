
#include <time.h>
#include "device/control/device_hub.h"
#include "device/device_child.h"
#include "util/util_converter.h"
#include <string.h>

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
    ConverterResult result;

    device_communication_message_init(hub, &out_message);

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_INFO,
                                                "%d\n",
                                                hub->device->state);

            break;
        }
        case MESSAGE_TYPE_SPAWN_DEVICE: {

            char *child_text_message = malloc(sizeof(char) * DEVICE_COMMUNICATION_MESSAGE_LENGTH);
            strcpy(child_text_message, in_message.message);

            char **tmp = device_communication_split_message_fields(&in_message);
            ConverterResult child_id = converter_string_to_long(tmp[0]);
            ConverterResult child_descriptor = converter_string_to_long(tmp[1]);

            if (child_id.error || child_descriptor.error) {
                device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_ERROR,
                                                    "Conversion Error: %s",
                                                    child_id.error_message);
                break;
            }

            control_device_fork(hub, child_id.data.Long, device_is_supported_by_id(child_descriptor.data.Long));

            /**
             * Send message to child in order to make it set new values
             */
            DeviceCommunicationMessage child_message;
            DeviceCommunication *data;

            device_communication_message_init(hub->device, &child_message);

            device_communication_message_modify(&child_message, child_id.data.Long, MESSAGE_TYPE_SET_INIT_VALUES,
                                                child_text_message);
            child_message.flag_force = true;
            child_message.flag_continue = false;
            /**
             *
             *
             *
             * Fix here
             *
             *
             *
             *
             *
             *
             */

            /**
             * Check if the device setted its values correctly
             */

            list_for_each(data, hub->devices) {
                if ((in_message = device_communication_write_message_with_ack(data, &child_message)).type ==
                    MESSAGE_TYPE_SET_INIT_VALUES) {
                    fprintf(stderr, "Successfully set child values\n");
                    break;
                }
            }

            /**
             * Notify success to parent
             */
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SPAWN_DEVICE, "");
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