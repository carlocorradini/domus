
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
    device_communication_message_init(hub->device, &out_message);

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_INFO,
                                                "%d\n",
                                                hub->device->state);

            break;
        }
        case MESSAGE_TYPE_SPAWN_DEVICE: {
            if(!list_is_empty(hub->devices)){
                ConverterResult child_descriptor_id;
                DeviceCommunicationMessage local_in_message_copy = in_message;


                child_descriptor_id = converter_string_to_long(device_communication_split_message_fields(&local_in_message_copy)[1]);

                DeviceCommunicationMessage check_child_message;
                DeviceCommunicationMessage in_child_message;

                device_communication_message_init(hub->device, &check_child_message);
                device_communication_message_modify(&check_child_message, hub->device->id, MESSAGE_TYPE_RECIPIENT_ID_MISLEADING, "");

                DeviceCommunication* device = (DeviceCommunication *) list_get_first(hub->devices);

                in_child_message = device_communication_write_message_with_ack(device, &check_child_message);

                if(!child_descriptor_id.error && in_child_message.id_device_descriptor == child_descriptor_id.data.Long){
                    device_child_set_device_to_spawn(in_message);
                    return;
                }
                else{
                    fprintf(stderr, "\tPlease attach a device with the same type\n");
                    device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_ERROR,
                                                        "Please attach a device with the same type");
                    device_communication_write_message(hub_communication, &out_message);
                    return;
                }
            } else {
                device_child_set_device_to_spawn(in_message);
                return;
            }
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