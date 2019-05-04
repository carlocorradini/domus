
#ifndef _DEVICE_COMMUNICATION_H
#define _DEVICE_COMMUNICATION_H

#include <unistd.h>
#include <sys/signal.h>
#include "device/device.h"

#define DEVICE_COMMUNICATION_CHILD_READ 0
#define DEVICE_COMMUNICATION_CHILD_WRITE 1
#define DEVICE_COMMUNICATION_READ_PIPE SIGUSR1
#define DEVICE_COMMUNICATION_MESSAGE_LENGTH 256
#define DEVICE_COMMUNICATION_MESSAGE_FIELDS_MAX 8
#define DEVICE_COMMUNICATION_MESSAGE_FIELDS_DELIMITER "\n"

/* Message types */
#define MESSAGE_TYPE_NO_MESSAGE 0
#define MESSAGE_TYPE_ERROR 1
#define MESSAGE_TYPE_TERMINATE 2
#define MESSAGE_TYPE_INFO 4
#define MESSAGE_TYPE_SET_ON 6
#define MESSAGE_TYPE_SPAWN_DEVICE 8
#define MESSAGE_TYPE_SET_INIT_VALUES_DEVICE 9
#define MESSAGE_TYPE_RECIPIENT_ID_MISLEADING 128
/* END Message types */

/* Message Status */
#define MESSAGE_RETURN_SUCCESS "SUCCESS"
#define MESSAGE_RETURN_NAME_ERROR "ERROR\nNAME"
#define MESSAGE_RETURN_VALUE_ERROR "ERROR\nVALUE"

#define MESSAGE_VALUE_LENGTH 64
/* END Message Status */

/** Struct Device Communication for storing information about a Communication between two processes
 */
typedef struct DeviceCommunication {
    pid_t pid;
    int com_read;
    int com_write;
} DeviceCommunication;

/**
 * Struct Device Communication message describing a message between two processes
 */
typedef struct DeviceCommunicationMessage {
    size_t type;

    size_t ctr_hop;

    size_t id_sender;
    size_t id_recipient;
    size_t id_device_descriptor;

    bool flag_force;
    bool flag_continue;

    char message[DEVICE_COMMUNICATION_MESSAGE_LENGTH];
} DeviceCommunicationMessage;

/**
 * Create and return a Device Communication Structure
 * @param pid The pid of the destination process
 * @param com_read The read pipe to read from
 * @param com_write The write pipe to read to
 * @return The new Device Communication, NULL otherwise
 */
DeviceCommunication *
new_device_communication(pid_t pid, int com_read, int com_write);

/**
 * Close the communication between the process
 * @param device_communication The Device Communication
 * @return true if communication has been closed, false otherwise
 */
bool device_communication_close_communication(DeviceCommunication *device_communication);

/**
 * Read a Message from the pipe given in device_communication
 * @param device_communication The Device Communication structure
 * @return The Message received
 */
DeviceCommunicationMessage device_communication_read_message(DeviceCommunication *device_communication);

/**
 * Write a message, notify the recipient and waits for a response(ACK)
 * @param device_communication The Device Communication structure
 * @param out_message The message to send
 * @return The message received(ACK)
 */
DeviceCommunicationMessage device_communication_write_message_with_ack(DeviceCommunication *device_communication,
                                                                       const DeviceCommunicationMessage *out_message);

/**
 * Write a message and waits for a response(ACK)
 * @param device_communication The Device Communication structure
 * @param out_message The message to send
 * @return The message received(ACK)
 */
DeviceCommunicationMessage device_communication_write_message_with_ack_silent(DeviceCommunication *device_communication,
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
 *  * The recipient
 *  * The type
 *  * The message
 * @param message The message to modify
 * @param id_recipient The id recipient
 * @param message_type The type to modify to
 * @param message_message The Message message to modify to
 * @param ... Format tags
 */
void device_communication_message_modify(DeviceCommunicationMessage *message, size_t id_recipient, size_t message_type,
                                         const char *message_message, ...);

/**
 * Modify a message:
 *  * The message
 * @param message The message to modify
 * @param message_message The Message message to modify to
 * @param ... Format tags
 */
void device_communication_message_modify_message(DeviceCommunicationMessage *message, const char *message_message, ...);

/**
 * Split a message into an array of fields from a message string
 *  Remember to free!
 * @param message The message to split from
 * @return An array of fields, NULL otherwise
 */
char **device_communication_split_message_fields(const DeviceCommunicationMessage *message);

#endif
