
#include <errno.h>
#include <string.h>
#include <time.h>
#include "device/device_communication.h"
#include "device/device_child.h"
#include "device/interaction/device_bulb.h"

/**
 *
 */
static Device *bulb = NULL;

static DeviceCommunication *bulb_communication = NULL;

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
    if (!device_check_device(bulb)) return false;
    bulb->state = state;
    return true;
}

static void bulb_message_handler(DeviceCommunicationMessage message) {
    DeviceCommunicationMessage message_write;
    message_write.type = 999;
    snprintf(message_write.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "Il dispositivo è attivo");
    device_communication_write_message(bulb_communication, &message_write);
}

void bulb_read_pipe(int signal_number) {
    if (signal_number == DEVICE_COMMUNICATION_READ_PIPE) {
        /* I need to read */
        device_communication_read_message(bulb_communication, bulb_message_handler);
    }
}

int main(int argc, char **args) {
    bulb = device_child_new_device(argc, args, new_bulb_registry, bulb_master_switch);
    bulb_communication = device_child_new_device_communication();
    signal(DEVICE_COMMUNICATION_READ_PIPE, bulb_read_pipe);

    while (true);

    /*DeviceCommunicationMessage device_message;
    int id = 0;
    while (true) {
        sleep(2);
        device_message.type = id++;
        snprintf(device_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH,
                 "Processo con id %ld e pid %d sta inviando il messaggio N° %d", bulb->id, getpid(), id);
        device_communication_write_message(bulb_communication, &device_message);
        if (id == 5) break;
    }*/

    return EXIT_SUCCESS;
}