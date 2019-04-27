#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "device/device_child.h"
#include "util/util_converter.h"

/**
 * The volatile variable for knowing if the process must continue or die
 */
static volatile sig_atomic_t _device_child_run = true;

/**
 * A pointer to the child Device Communication for easy of use
 */
static DeviceCommunication *device_child_communication = NULL;

/**
 * A pointer to the child Device for easy of use
 */
static Device *device_child = NULL;

/**
 * Function for handling signal when receiving a message
 * @param signal_number The signal number to identify as macro DEVICE_COMMUNICATION_READ_PIPE
 */
static void device_child_read_pipe(int signal_number);

/**
 * Middleware message handler for import messages that must be handled
 * @param in_message The incoming message
 */
static void devive_child_middleware_message_handler(DeviceCommunicationMessage in_message);

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

    result = converter_string_to_long(args[0]);
    if (result.error) {
        fprintf(stderr, "Conversion Error: %s\n", result.error_message);
        exit(EXIT_FAILURE);
    }

    device_child = new_device(getpid(), result.data.Long, DEVICE_STATE, registry);

    return device_child;
}

DeviceCommunication *
device_child_new_device_communication(int argc, char **args, void (*message_handler)(DeviceCommunicationMessage)) {
    ConverterResult result;
    if (!device_child_check_args(argc, args)) return NULL;
    if (message_handler == NULL || device_child_message_handler != NULL || device_child_communication != NULL)
        return NULL;

    result = converter_string_to_long(args[1]);
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

void device_child_run(void (*do_on_wake_up)(void)) {
    while (_device_child_run) {
        pause();
        if (do_on_wake_up != NULL) do_on_wake_up();
    }
}

static void device_child_read_pipe(int signal_number) {
    if (signal_number == DEVICE_COMMUNICATION_READ_PIPE) {
        if (device_child_communication == NULL || device_child_message_handler == NULL)
            return;

        devive_child_middleware_message_handler(device_communication_read_message(device_child_communication));
    }
}

static void devive_child_middleware_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    out_message.id_sender = device_child->id;

    switch (in_message.type) {
        case MESSAGE_TYPE_TERMINATE: {
            out_message.type = MESSAGE_TYPE_TERMINATE;
            snprintf(out_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "%d", true);
            /* Stop the Device */
            _device_child_run = false;
            break;
        }
        default: {
            device_child_message_handler(in_message);
            return;
        }
    }

    device_communication_write_message(device_child_communication, &out_message);
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
