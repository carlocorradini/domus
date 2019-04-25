
#include <string.h>
#include <time.h>
#include "device/device_communication.h"
#include "device/device_child.h"
#include "device/interaction/device_bulb.h"

/**
 * The bulb Device
 */
static Device *bulb = NULL;

/**
 * The Device Communication for Bulb
 */
static DeviceCommunication *bulb_communication = NULL;

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

bool bulb_master_switch(bool state) {
    BulbRegistry *bulb_registry;
    if (!device_check_device(bulb)) return false;
    if (bulb->state == state) return true;

    bulb_registry = (BulbRegistry *) bulb->registry;

    bulb->state = state;
    (bulb->state) ? time(&bulb_registry->start) : (bulb_registry->start = 0);

    return true;
}

static void bulb_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;

    switch (in_message.type) {
        case MESSAGE_TYPE_IS_ON: {
            out_message.type = MESSAGE_TYPE_IS_ON;
            snprintf(out_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "%d", bulb->state);
            device_communication_write_message(bulb_communication, &out_message);
            break;
        }
        case MESSAGE_TYPE_SET_ON: {
            out_message.type = MESSAGE_TYPE_SET_ON;
            snprintf(out_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "%d", bulb_master_switch());
            device_communication_write_message(bulb_communication, &out_message);
            exit(EXIT_SUCCESS);
        }
        default: {
            out_message.type = MESSAGE_TYPE_ERROR;
            snprintf(out_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH,
                     "Bulb received something wrong: {%d, %s}",
                     in_message.type, in_message.message);
            device_communication_write_message(bulb_communication, &out_message);
            break;
        }
    }
}

int main(int argc, char **args) {
    bulb = device_child_new_device(argc, args, new_bulb_registry, bulb_master_switch);
    bulb_communication = device_child_new_device_communication(bulb_message_handler);

    while (true);

    return EXIT_SUCCESS;
}