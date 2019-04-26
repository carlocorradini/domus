
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "device/device_child.h"
#include "util/util_converter.h"

/**
 * Function for handling signal when receiving a message
 * @param signal_number The signal number to identify as macro DEVICE_COMMUNICATION_READ_PIPE
 */
static void device_child_read_pipe(int signal_number);

/**
 * A pointer to the child Device Communication fro easy of use
 */
static DeviceCommunication *device_child_communication = NULL;

/**
 * A function pointer to the child Message Handler for easy of use
 */
static void (*device_child_message_handler)(DeviceCommunicationMessage) = NULL;

/**
 * Check child arguments if correspond to the minimum required macro DEVICE_CHILD_ARGS_LENGTH
 * @param argc The number of arguments
 * @param args The arguments
 * @return true if args is valid, false otherwise
 */
static bool device_child_check_args(int argc, char **args);

Device *device_child_new_device(int argc, char **args, void *registry) {
    ConverterResult result;
    if (!device_child_check_args(argc, args)) return NULL;

    result = converter_char_to_long(args[0]);
    if (result.error) {
        fprintf(stderr, "Conversion Error: %s\n", result.error_message);
        exit(EXIT_FAILURE);
    }

    return new_device(getpid(), result.data.Long, DEVICE_STATE, registry);
}

DeviceCommunication *
device_child_new_device_communication(int argc, char **args, void (*message_handler)(DeviceCommunicationMessage)) {
    ConverterResult result;
    if (!device_child_check_args(argc, args)) return NULL;
    if (message_handler == NULL || device_child_message_handler != NULL || device_child_communication != NULL)
        return NULL;

    result = converter_char_to_long(args[1]);
    if (result.error) {
        fprintf(stderr, "Conversion Error: %s\n", result.error_message);
        exit(EXIT_FAILURE);
    }

    device_child_message_handler = message_handler;
    signal(DEVICE_COMMUNICATION_READ_PIPE, device_child_read_pipe);

    device_child_communication = new_device_communication(result.data.Long, getppid(), NULL,
                                                          DEVICE_COMMUNICATION_CHILD_READ,
                                                          DEVICE_COMMUNICATION_CHILD_WRITE);
    return device_child_communication;
}

static void device_child_read_pipe(int signal_number) {
    if (signal_number == DEVICE_COMMUNICATION_READ_PIPE) {
        /* I need to read */
        if (device_child_communication == NULL || device_child_message_handler == NULL)
            return;
        device_communication_read_message(device_child_communication, device_child_message_handler);
    }
}

static bool device_child_check_args(int argc, char **args) {
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