
#ifndef DEVICE_COMMUNICATION_H
#define DEVICE_COMMUNICATION_H

#include <unistd.h>
#include "device/device.h"

#define DEVICE_COMMUNICATION_CHILD_READ 0
#define DEVICE_COMMUNICATION_CHILD_WRITE 1
#define DEVICE_COMMUNICATION_MESSAGE_LENGTH 256

typedef struct DeviceCommunication {
    pid_t pid;
    const DeviceDescriptor *device_descriptor;
    int com_read;
    int com_write;
} DeviceCommunication;

typedef struct DeviceCommunicationMessage {
    unsigned short int type;
    char message[DEVICE_COMMUNICATION_MESSAGE_LENGTH];
} DeviceCommunicationMessage;

DeviceCommunication *
new_device_communication(pid_t pid, const DeviceDescriptor *device_descriptor, int com_read, int com_write);

bool device_communication_read_message(const DeviceCommunication *device_communication);

bool device_communication_write_message(const DeviceCommunication *device_communication, const DeviceCommunicationMessage *message);

void device_communication_notify(pid_t pid);

#endif
