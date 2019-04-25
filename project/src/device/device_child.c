
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "device/device_child.h"

bool device_child_check_args(int argc, char **args) {
    if (args == NULL) {
        fprintf(stderr, "Device Child Args: args cannot be NULL\n");
        exit(EXIT_FAILURE);
    }
    if (argc != DEVICE_CHILD_ARGS_LENGTH) {
        fprintf(stderr, "Device Child Args: %d required, %d passed\n", DEVICE_CHILD_ARGS_LENGTH, argc);
        exit(EXIT_FAILURE);
    }

    return true;
}

Device *device_child_new_device(int argc, char **args, void *registry, bool (*master_switch)(bool)) {
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

DeviceCommunication *device_child_new_device_communication(void) {
    return new_device_communication(getppid(), NULL,
                                    DEVICE_COMMUNICATION_CHILD_READ,
                                    DEVICE_COMMUNICATION_CHILD_WRITE);
}

