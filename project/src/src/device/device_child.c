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
 * A pointer to the child Device for easy of use
 */
static Device *device_child = NULL;

/**
 * A pointer to the child Control Device for easy of use
 */
static ControlDevice *control_device_child = NULL;

/**
 * A pointer to the child Device Communication for easy of use
 */
static DeviceCommunication *device_child_communication = NULL;

/**
 * Function for handling signal when receiving a message
 * @param signal_number The signal number to identify as macro DEVICE_COMMUNICATION_READ_PIPE
 */
static void device_child_read_pipe(int signal_number);

/**
 * Device only
 * Middleware message handler for messages that must be handled before forwarding
 * @param in_message The incoming message
 */
static void devive_child_middleware_message_handler(DeviceCommunicationMessage in_message);

/**
 * Control Device only
 * Middleware message handler for messages that must be handled before forwarding
 */
static void control_devive_child_middleware_message_handler();

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

void device_child_run(void (*do_on_wake_up)(void)) {
    while (_device_child_run) {
        pause();
        if (do_on_wake_up != NULL) do_on_wake_up();
    }
}

static void device_child_read_pipe(int signal_number) {
    if (signal_number == DEVICE_COMMUNICATION_READ_PIPE) {
        if (device_child_communication == NULL || device_child_message_handler == NULL) return;
        if (control_device_child == NULL && device_child == NULL) {
            fprintf(stderr, "Device Child Read Pipe: Init Something\n");
            exit(EXIT_FAILURE);
        }

        if (control_device_child != NULL && device_child == NULL) {
            /* Middleware for Control Device */
            control_devive_child_middleware_message_handler();
        } else if (device_child != NULL && control_device_child == NULL) {
            /* Middleware for Device */
            devive_child_middleware_message_handler(device_communication_read_message(device_child_communication));
        }
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

Device *device_child_new_device(int argc, char **args, void *registry) {
    ConverterResult device_id;
    ConverterResult device_descriptor_id;
    const DeviceDescriptor *device_descriptor;
    if (!device_child_check_args(argc, args)) return NULL;
    if (registry == NULL) return NULL;
    /* Init Supported Devices if not */
    device_init();

    device_id = converter_string_to_long(args[0]);
    device_descriptor_id = converter_string_to_long(args[1]);

    if (device_id.error) {
        fprintf(stderr, "Device ID Conversion Error: %s\n", device_id.error_message);
        exit(EXIT_FAILURE);
    } else if (device_descriptor_id.error) {
        fprintf(stderr, "Device Descriptor ID Conversion Error: %s\n", device_descriptor_id.error_message);
        exit(EXIT_FAILURE);
    } else if ((device_descriptor = device_is_supported_by_id(device_descriptor_id.data.Long)) == NULL) {
        fprintf(stderr, "Cannot find a Device Descriptor with ID %ld\n", device_descriptor_id.data.Long);
        exit(EXIT_FAILURE);
    } else if (device_descriptor->control_device) {
        fprintf(stderr, "Cannot create a simple Device that is a Control Device\n");
        exit(EXIT_FAILURE);
    }

    device_child = new_device(device_id.data.Long, device_descriptor_id.data.Long, DEVICE_STATE, registry);

    return device_child;
}

DeviceCommunication *
device_child_new_device_communication(int argc, char **args, void (*message_handler)(DeviceCommunicationMessage)) {
    if (!device_child_check_args(argc, args)) return NULL;
    if (message_handler == NULL || device_child_message_handler != NULL) return NULL;

    device_child_message_handler = message_handler;
    signal(DEVICE_COMMUNICATION_READ_PIPE, device_child_read_pipe);

    device_child_communication = new_device_communication(getppid(), DEVICE_COMMUNICATION_CHILD_READ,
                                                          DEVICE_COMMUNICATION_CHILD_WRITE);
    return device_child_communication;
}

static void devive_child_middleware_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    if (device_child == NULL || device_child_communication == NULL) return;

    device_communication_message_init(device_child, &out_message);

    /* Incoming message is not for this Device */
    if (!in_message.flag_force && in_message.id_recipient != device_child->id) {
        device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_RECIPIENT_ID_MISLEADING,
                                            "");
        device_communication_write_message(device_child_communication, &out_message);
        return;
    }

    switch (in_message.type) {
        case MESSAGE_TYPE_TERMINATE: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_TERMINATE,
                                                "%ld\n", device_child->device_descriptor->id);
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

ControlDevice *device_child_new_control_device(int argc, char **args, void *registry) {
    ConverterResult control_device_id;
    ConverterResult control_device_descriptor_id;
    const DeviceDescriptor *device_descriptor;
    if (!device_child_check_args(argc, args)) return NULL;
    if (registry == NULL) return NULL;
    /* Init Supported Devices if not */
    device_init();

    control_device_id = converter_string_to_long(args[0]);
    control_device_descriptor_id = converter_string_to_long(args[1]);

    if (control_device_id.error) {
        fprintf(stderr, "Control Device ID Conversion Error: %s\n", control_device_id.error_message);
        exit(EXIT_FAILURE);
    } else if (control_device_descriptor_id.error) {
        fprintf(stderr, "Control Device Descriptor ID Conversion Error: %s\n",
                control_device_descriptor_id.error_message);
        exit(EXIT_FAILURE);
    } else if ((device_descriptor = device_is_supported_by_id(control_device_descriptor_id.data.Long)) == NULL) {
        fprintf(stderr, "Cannot find a Device Descriptor with ID %ld\n", control_device_descriptor_id.data.Long);
        exit(EXIT_FAILURE);
    } else if (!device_descriptor->control_device) {
        fprintf(stderr, "Cannot create a Control Device that is a simple Device\n");
        exit(EXIT_FAILURE);
    }

    control_device_child = new_control_device(
            new_device(control_device_id.data.Long, control_device_descriptor_id.data.Long, DEVICE_STATE, registry));

    return control_device_child;
}

DeviceCommunication *device_child_new_control_device_communication(int argc, char **args, void (*message_handler)(
        DeviceCommunicationMessage)) {
    if (control_device_child == NULL) return NULL;

    return device_child_new_device_communication(argc, args, message_handler);
}

static void control_devive_child_middleware_message_handler() {
    DeviceCommunication *data;
    DeviceCommunicationMessage in_message;
    DeviceCommunicationMessage out_message;
    if (control_device_child == NULL || device_child_communication == NULL) return;

    in_message = device_communication_read_message(device_child_communication);
    device_communication_message_init(control_device_child->device, &out_message);

    if (in_message.type == MESSAGE_TYPE_NO_MESSAGE) {
        /* Read Child Messages */
        list_for_each(data, control_device_child->devices) {
            in_message = device_communication_read_message(data);
            if (in_message.type != MESSAGE_TYPE_NO_MESSAGE) {
                /* Message from child found, break */
                break;
            }
        }
    }

    switch (in_message.type) {
        case MESSAGE_TYPE_NO_MESSAGE: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_ERROR,
                                                "Read Signal Received but no Message found");
            break;
        }
        case MESSAGE_TYPE_TERMINATE: {
            out_message.type = MESSAGE_TYPE_TERMINATE;
            list_for_each(data, control_device_child->devices) {
                device_communication_write_message_with_ack(data, &out_message);
            }
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
