
#include <errno.h>
#include <signal.h>
#include "device/device_communication.h"
#include "util/util_printer.h"

DeviceCommunication *
new_device_communication(pid_t pid, const DeviceDescriptor *device_descriptor, int com_read, int com_write) {
    DeviceCommunication *device_communication = (DeviceCommunication *) malloc(sizeof(DeviceCommunication));
    if (device_communication == NULL) {
        perror("Device Communication Memory Allocation");
        exit(EXIT_FAILURE);
    }

    device_communication->pid = pid;
    device_communication->device_descriptor = device_descriptor;
    device_communication->com_read = com_read;
    device_communication->com_write = com_write;

    return device_communication;
}

bool device_communication_read_message(const DeviceCommunication *device_communication,
                                       void (*message_handler)(DeviceCommunicationMessage message)) {
    DeviceCommunicationMessage message;
    if (device_communication == NULL || device_communication->com_read < 0) return false;

    while (true) {
        switch (read(device_communication->com_read, &message, sizeof(DeviceCommunicationMessage))) {
            case -1: {
                /* Empty or Error */
                if (errno == EAGAIN) {
                    println("\tPipe of %ld pid is empty", device_communication->pid);
                    return true;
                } else {
                    perror("Error pipe read");
                    exit(EXIT_FAILURE);
                }
            }
            case 0: {
                /* Process is terminated */
                println("\tProcess with pid %ld has terminated, closing", device_communication->pid);
                close(device_communication->com_read);
                close(device_communication->com_write);
                return true;
            }
            default: {
                /* Message Found */
                message_handler(message);
                break;
            }
        }
    }
}

bool device_communication_write_message(const DeviceCommunication *device_communication,
                                        const DeviceCommunicationMessage *message) {
    if (device_communication == NULL || message == NULL || device_communication->com_write < 0) return false;

    write(device_communication->com_write, message, sizeof(DeviceCommunicationMessage));
    device_communication_notify(device_communication->pid);

    return true;
}

void device_communication_notify(pid_t pid) {
    kill(pid, DEVICE_COMMUNICATION_READ_PIPE);
}