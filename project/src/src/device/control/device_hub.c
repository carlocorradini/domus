
#include <time.h>
#include "device/control/device_hub.h"
#include "device/device_child.h"
#include "util/util_converter.h"
#include "util/util_string_handler.h"

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
            result = converter_bool_to_string(hub->device->state);
            time_t open_time = ((HubRegistry *) hub->device->registry)->start;
            time_t end_time = time(NULL);
            double difference = (open_time == 0) ? 0 : difftime(end_time, open_time);

            device_communication_message_modify(&out_message, MESSAGE_TYPE_INFO,
                                                "ID:%5ld | STATE:%10s | USAGE_TIME:%8.2f seconds",
                                                hub->device->id,
                                                result.data.String,
                                                difference
            );

            break;
        }
        case MESSAGE_TYPE_RESPAWN_DEVICE: {
            char **tmp = string_to_string_array(in_message.message);
            ConverterResult child_id = converter_string_to_long(tmp[0]);

            if (child_id.error) {
                device_communication_message_modify(&out_message, MESSAGE_TYPE_ERROR, "Conversion Error: %s",
                                                    child_id.error_message);
                break;
            }
            control_device_fork(hub, child_id.data.Long, new_device_descriptor("bulb", "A bulb", "./bulb"));
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