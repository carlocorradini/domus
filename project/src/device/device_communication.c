
#include <errno.h>
#include <signal.h>
#include "device/device_communication.h"

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

bool device_communication_read_message(const DeviceCommunication *device_communication) {
    DeviceCommunicationMessage message;
    int read_result;
    if (device_communication == NULL || device_communication->com_read < 0) return false;

    while ((read_result = read(device_communication->com_read, &message, sizeof(DeviceCommunicationMessage))) != 0) {

        switch (read_result) {
            case -1: {
                /* Empty or Error */
                if (errno != EAGAIN) {
                    perror("Error pipe read");
                    exit(EXIT_FAILURE);
                }
                break;
            }
            case 0: {
                /* End of Messages */
                return true;
            }
            default: {
                /* Message Found */
                printf("\t{%d, %s}\n", message.type, message.message);
                break;
            }
        }
    }

    return true;
}

bool device_communication_write_message(const DeviceCommunication *device_communication,
                                        const DeviceCommunicationMessage *message) {
    if (device_communication == NULL || message == NULL
        || device_communication->com_write < 0 || device_communication->pid < 0)
        return false;

    write(device_communication->com_write, message, sizeof(DeviceCommunicationMessage));
    device_communication_notify(device_communication->pid);
    return true;
}

void device_communication_notify(pid_t pid) {
    kill(pid, SIGUSR1);
}