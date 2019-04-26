
#ifndef DEVICE_COMMUNICATION_H
#define DEVICE_COMMUNICATION_H

#include <unistd.h>
#include <sys/signal.h>
#include "device/device.h"

#define DEVICE_COMMUNICATION_CHILD_READ 0
#define DEVICE_COMMUNICATION_CHILD_WRITE 1
#define DEVICE_COMMUNICATION_READ_PIPE SIGUSR1
#define DEVICE_COMMUNICATION_MESSAGE_LENGTH 256

/* Message types */
#define MESSAGE_TYPE_DEBUG 0
#define MESSAGE_TYPE_ERROR 1
#define MESSAGE_TYPE_TERMINATE 2
#define MESSAGE_TYPE_INFO 3
/* EDN Message types */

/**
 *
 */
typedef struct DeviceCommunication {
    size_t id;
    pid_t pid;
    const DeviceDescriptor *device_descriptor;
    int com_read;
    int com_write;
} DeviceCommunication;

/**
 *
 */
typedef struct DeviceCommunicationMessage {
    size_t id_sender;
    unsigned short int type;
    char message[DEVICE_COMMUNICATION_MESSAGE_LENGTH];
} DeviceCommunicationMessage;

/**
 *
 * @param id
 * @param pid
 * @param device_descriptor
 * @param com_read
 * @param com_write
 * @return
 */
DeviceCommunication *
new_device_communication(size_t id, pid_t pid, const DeviceDescriptor *device_descriptor, int com_read, int com_write);

/**
 *
 * @param device_communication
 * @return
 */
DeviceCommunicationMessage device_communication_read_message(const DeviceCommunication *device_communication);

/**
 *
 * @param device_communication
 * @param message
 * @return
 */
DeviceCommunicationMessage device_communication_write_message_with_ack(const DeviceCommunication *device_communication,
                                                                       const DeviceCommunicationMessage *message);

/**
 *
 * @param device_communication
 * @param out_message
 */
void device_communication_write_message(const DeviceCommunication *device_communication,
                                        const DeviceCommunicationMessage *out_message);

#endif
