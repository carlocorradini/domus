
#include <errno.h>
#include <signal.h>
#include "device/device_communication.h"

DeviceCommunication *
new_device_communication(pid_t pid, const DeviceDescriptor *device_descriptor, int com_read, int com_write) {
    DeviceCommunication *device_communication;
    if (device_descriptor == NULL) return NULL;

    device_communication = (DeviceCommunication *) malloc(sizeof(DeviceCommunication));
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

void device_communication_read(int com_read) {
    DeviceCommunicationMessage message;
    int read_result;
    if (com_read < 0) return;

    while ((read_result = read(com_read, &message, sizeof(DeviceCommunicationMessage))) != 0) {
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
                return;
            }
            default: {
                /* Message Found */
                printf("\t{%d, %s}\n", message.type, message.message);
                break;
            }
        }
    }
}

void device_communication_write(int com_write, const DeviceCommunicationMessage *message) {
    if (message == NULL) return;
    if (com_write < 0) return;
    write(com_write, message, sizeof(DeviceCommunicationMessage));
}

void device_communication_notify(pid_t pid) {
    kill(pid, SIGUSR1);
}