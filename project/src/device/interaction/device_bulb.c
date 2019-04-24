
#include <errno.h>
#include <string.h>
#include <time.h>
#include "device/device_communication.h"
#include "device/interaction/device_bulb.h"

/**
 *
 */
static Device *bulb = NULL;

static bool device_child_check_args(int argc, char **args) {
    if (args == NULL) {
        fprintf(stderr, "Device Child Args: args cannot be NULL\n");
        exit(EXIT_FAILURE);
    }
    if (argc != DEVICE_CHILD_ARGS_LENGTH) {
        fprintf(stderr, "Device Child Args: %d required, %d passed\n", DEVICE_CHILD_ARGS_LENGTH, argc);
        exit(EXIT_FAILURE);
    }

    /**/

    return true;
}

static Device *device_child_new_device(int argc, char **args, void *registry, bool (*master_switch)(bool)) {
    size_t id;
    const char *id_str;
    char *id_str_end = NULL;
    if (!device_child_check_args(argc, args)) return NULL;

    id_str = args[0];
    id_str_end = NULL;
    errno = 0;
    id = strtol(id_str, &id_str_end, 10);

    if (id_str == id_str_end) {
        fprintf(stderr, "Device Child Device Conversion Error: No digits found\n");
        exit(EXIT_FAILURE);
    } else if (errno == EINVAL) {
        fprintf(stderr, "Device Child Device Conversion Error: Base contains unsupported value\n");
        exit(EXIT_FAILURE);
    } else if (errno != 0 && id == 0) {
        fprintf(stderr, "Device Child Device Conversion Error: Unspecified error occurred\n");
        exit(EXIT_FAILURE);
    } else if (errno == 0 && id_str && *id_str_end != 0) {
        fprintf(stderr, "Device Child Device Conversion Error: Additional characters remain\n");
        exit(EXIT_FAILURE);
    } else if (id <= 0) {
        fprintf(stderr, "Device Child Device: id %ld is invalid\n", id);
        exit(EXIT_FAILURE);
    }

    return new_device(getpid(), id, DEVICE_STATE, registry, master_switch);
}

typedef struct BulbRegistry {
    time_t start;
} BulbRegistry;

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

static bool bulb_master_switch(bool state) {
    if (!device_check_device(bulb)) return false;
    bulb->state = state;
    return true;
}

#include <sys/signal.h>

int main(int argc, char **args) {
    bulb = device_child_new_device(argc, args, new_bulb_registry, bulb_master_switch);
    const DeviceCommunication *device_communication = new_device_communication(getppid(), NULL,
                                                                               DEVICE_COMMUNICATION_CHILD_READ,
                                                                               DEVICE_COMMUNICATION_CHILD_WRITE);
    DeviceCommunicationMessage device_message;

    strncpy(device_message.message, "CI ARRIVO", DEVICE_COMMUNICATION_MESSAGE_LENGTH);
    device_message.type = bulb->id;

    device_communication_write_message(device_communication, &device_message);

    return EXIT_SUCCESS;
}