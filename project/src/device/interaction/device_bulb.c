
#include <string.h>
#include <time.h>
#include "device/device_communication.h"
#include "device/device_child.h"
#include "device/interaction/device_bulb.h"
#include "util/util_converter.h"

/**
 * The bulb Device
 */
static Device *bulb = NULL;

/**
 * The Device Communication for Bulb
 */
static DeviceCommunication *bulb_communication = NULL;

/**
 * Set the bulb switch state
 * @param state the state to set
 * @return true if successful, false otherwise
 */
static bool bulb_set_switch_state(char name[], bool *state);

/**
 * Handle the incoming message
 * @param in_message The received message
 */
static void bulb_message_handler(DeviceCommunicationMessage in_message);

BulbRegistry *new_bulb_registry(void) {
    BulbRegistry *bulb_registry;
    if (device_check_device(bulb)) return NULL;

    bulb_registry = (BulbRegistry *) malloc(sizeof(BulbRegistry));
    if (bulb_registry == NULL) {
        perror("Bulb Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }

    time(&bulb_registry->start);

    return bulb_registry;
}

static bool bulb_set_switch_state(char name[], bool *state) {
    BulbRegistry *bulb_registry;
    DeviceSwitch *bulb_switch;

    if (list_contains(bulb->switches, name)) {
        if (bulb->state == (bool) state) return true;

        bulb_switch = list_get(bulb->switches, (size_t) list_get_index(bulb->switches, name));
        bulb_switch->state = state;

        bulb->state = (bool) state;
        bulb_registry = (BulbRegistry *) bulb->registry;
        (state) ? time(&bulb_registry->start) : (bulb_registry->start = 0);

        return true;
    }
    return false;
}

static void bulb_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    ConverterResult result;

    out_message.id_sender = bulb->id;

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            out_message.type = MESSAGE_TYPE_INFO;
            snprintf(out_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "%ld     %s     %s",
                     bulb->id,
                     "bulbo",
                     "semplice bulbo");
            break;
        }
        default: {
            out_message.type = MESSAGE_TYPE_ERROR;
            snprintf(out_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "{%d, %s}", in_message.type,
                     in_message.message);
            break;
        }
    }

    device_communication_write_message(bulb_communication, &out_message);
}

int main(int argc, char **args) {
    bulb = device_child_new_device(argc, args, new_bulb_registry);
    list_add_last(bulb->switches, new_device_switch("turn", DEVICE_STATE, bulb_set_switch_state));

    bulb_communication = device_child_new_device_communication(argc, args, bulb_message_handler);

    device_child_run(NULL);

    return EXIT_SUCCESS;
}