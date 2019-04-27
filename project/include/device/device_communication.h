
#ifndef DEVICE_COMMUNICATION_H
#define DEVICE_COMMUNICATION_H

#include <unistd.h>
#include <sys/signal.h>
#include "device/device.h"

#define DEVICE_COMMUNICATION_CHILD_READ 0
#define DEVICE_COMMUNICATION_CHILD_WRITE 1
#define DEVICE_COMMUNICATION_READ_PIPE SIGUSR1
#define DEVICE_COMMUNICATION_MESSAGE_LENGTH 256

#define MESSAGE_DELIMITER "\n"

/* Message types */
#define MESSAGE_TYPE_DEBUG 0
#define MESSAGE_TYPE_ERROR 1
#define MESSAGE_TYPE_TERMINATE 2
#define MESSAGE_TYPE_INFO 3
#define MESSAGE_TYPE_SET_ON 4
/* END Message types */

/* Message Status */
#define MESSAGE_RETURN_SUCCESS "SUCCESS"
#define MESSAGE_RETURN_NAME_ERROR "ERROR\nNAME"
#define MESSAGE_RETURN_VALUE_ERROR "ERROR\nVALUE"
/* END Message Status */

/** Struct Device Communication for storing information about a Communication between two processes
 */
typedef struct DeviceCommunication {
    size_t id;
    pid_t pid;
    const DeviceDescriptor *device_descriptor;
    int com_read;
    int com_write;
} DeviceCommunication;

typedef unsigned short int message_t;

/**
 * Struct Device Communication message describing a message between two processes
 */
typedef struct DeviceCommunicationMessage {
    size_t id_sender;
    message_t type;
    char message[DEVICE_COMMUNICATION_MESSAGE_LENGTH];
} DeviceCommunicationMessage;

/**
 * Create and return a Device Communication Structure
 * @param id The id of the destination process
 * @param pid The pid of the destination process
 * @param device_descriptor The Device Descriptor of the destination process
 * @param com_read The read pipe to read from
 * @param com_write The write pipe to read to
 * @return The new Device Communication, NULL otherwise
 */
DeviceCommunication *
new_device_communication(size_t id, pid_t pid, const DeviceDescriptor *device_descriptor, int com_read, int com_write);

/**
 * Read a Message from the pipe given in device_communication
 * @param device_communication The Device Communication structure
 * @return The Message received
 */
DeviceCommunicationMessage device_communication_read_message(const DeviceCommunication *device_communication);


/**
 * Write a message and waits for a response(ACK)
 * @param device_communication The Device Communication structure
 * @param out_message The message to send
 * @return The message received(ACK)
 */
DeviceCommunicationMessage device_communication_write_message_with_ack(const DeviceCommunication *device_communication,
                                                                       const DeviceCommunicationMessage *out_message);

/**
 * Write a message
 * @param device_communication The Device Communication structure
 * @param out_message The message to send
 */
void device_communication_write_message(const DeviceCommunication *device_communication,
                                        const DeviceCommunicationMessage *out_message);

/**
 * Initialize a Message
 * @param device The device to get the id from
 * @param message The message to initialize
 */
void device_communication_message_init(const Device *device, DeviceCommunicationMessage *message);

/**
 * Modify a message:
 *  * The type
 *  * The message
 * @param message The message to modify
 * @param message_type The type to modify to
 * @param message_message The Message message to modify to
 * @param ... Format tags
 */
void device_communication_message_modify(DeviceCommunicationMessage *message, message_t message_type,
                                         const char *message_message, ...);

/**
 * Modify a message:
 *  * The message
 * @param message The message to modify
 * @param message_message The Message message to modify to
 * @param ... Format tags
 */
void device_communication_message_modify_message(DeviceCommunicationMessage *message, const char *message_message, ...);

#endif
