
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include "device/device_communication.h"
#include "util/util_printer.h"

/**
 * Notify from writer to reader an incoming message
 * @param pid The pid of the reader process
 */
static void device_communication_notify(pid_t pid);

/**
 * Modify a Message message
 * @param message The message to change
 * @param message_message The message to change to
 * @param args Format tags
 */
static void
_device_communication_modify_message(DeviceCommunicationMessage *message, const char *message_message, va_list args);

DeviceCommunication *
new_device_communication(size_t id, pid_t pid, const DeviceDescriptor *device_descriptor, int com_read, int com_write) {
    DeviceCommunication *device_communication = (DeviceCommunication *) malloc(sizeof(DeviceCommunication));
    if (device_communication == NULL) {
        perror("Device Communication Memory Allocation");
        exit(EXIT_FAILURE);
    }

    device_communication->id = id;
    device_communication->pid = pid;
    device_communication->device_descriptor = device_descriptor;
    device_communication->com_read = com_read;
    device_communication->com_write = com_write;

    return device_communication;
}

DeviceCommunicationMessage device_communication_read_message(const DeviceCommunication *device_communication) {
    DeviceCommunicationMessage in_message;
    in_message.type = MESSAGE_TYPE_ERROR;

    if (device_communication == NULL) {
        snprintf(in_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH,
                 "Device Communication has not been initialized");
        return in_message;
    }

    switch (read(device_communication->com_read, &in_message, sizeof(DeviceCommunicationMessage))) {
        case -1: {
            /* Empty or Error */
            if (errno == EAGAIN) {
                in_message.type = MESSAGE_TYPE_NO_MESSAGE;
            } else {
                perror("Error pipe read");
                exit(EXIT_FAILURE);
            }

            break;
        }
        case 0: {
            /* Process is terminated, close */
            if (close(device_communication->com_read) == -1
                || close(device_communication->com_write) == -1
                || waitpid(device_communication->pid, 0, 0) == -1) {
                perror("Error closing pipe in Read Message");
                exit(EXIT_FAILURE);
            }

            break;
        }
        default: {
            /* Message was correctly received */
            break;
        }
    }

    return in_message;
}

DeviceCommunicationMessage device_communication_write_message_with_ack(const DeviceCommunication *device_communication,
                                                                       const DeviceCommunicationMessage *out_message) {
    DeviceCommunicationMessage in_message;
    in_message.type = MESSAGE_TYPE_ERROR;

    if (device_communication == NULL || out_message == NULL) {
        snprintf(in_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH,
                 "Device Communication OR Message has not been initialized");
        return in_message;
    }

    device_communication_write_message(device_communication, out_message);

    device_communication_notify(device_communication->pid);

    return device_communication_read_message(device_communication);
}

void device_communication_write_message(const DeviceCommunication *device_communication,
                                        const DeviceCommunicationMessage *out_message) {
    if (device_communication == NULL) return;

    if (write(device_communication->com_write, out_message, sizeof(DeviceCommunicationMessage)) == -1) {
        perror("Error Writing Message");
        exit(EXIT_FAILURE);
    }
}

static void device_communication_notify(pid_t pid) {
    kill(pid, DEVICE_COMMUNICATION_READ_PIPE);
}

void device_communication_message_init(const Device *device, DeviceCommunicationMessage *message) {
    if (device == NULL || message == NULL) return;

    message->id_sender = device->id;
    message->type = MESSAGE_TYPE_ERROR;
    snprintf(message->message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "Message has not been initialized");
}

static void
_device_communication_modify_message(DeviceCommunicationMessage *message, const char *message_message, va_list args) {
    if (message == NULL || message_message == NULL) return;

    vsnprintf(message->message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, message_message, args);
}

void device_communication_message_modify(DeviceCommunicationMessage *message, message_t message_type,
                                         const char *message_message, ...) {
    if (message == NULL || message_message == NULL) return;

    message->type = message_type;
    va_list args;
    va_start(args, message_message);
    _device_communication_modify_message(message, message_message, args);
    va_end(args);
}

void
device_communication_message_modify_message(DeviceCommunicationMessage *message, const char *message_message, ...) {
    if (message == NULL || message_message == NULL) return;

    va_list args;
    va_start(args, message_message);
    _device_communication_modify_message(message, message_message, args);
    va_end(args);
}