
#include <errno.h>
#include <signal.h>
#include "device/device_communication.h"
#include "util/util_printer.h"

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

#include <sys/wait.h>

bool device_communication_read_message(const DeviceCommunication *device_communication,
                                       void (*message_handler)(DeviceCommunicationMessage message)) {
    DeviceCommunicationMessage message;
    if (device_communication == NULL || device_communication->com_read < 0) return false;

    while (true) {
        switch (read(device_communication->com_read, &message, sizeof(DeviceCommunicationMessage))) {
            case -1: {
                /* Empty or Error */
                if (errno == EAGAIN) {
                    /* No Error */
                    return true;
                } else {
                    perror("Error pipe read");
                    exit(EXIT_FAILURE);
                }
            }
            case 0: {
                /* Process is terminated */
                close(device_communication->com_read);
                close(device_communication->com_write);
                waitpid(device_communication->pid, 0, 0);
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