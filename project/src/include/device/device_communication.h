
#ifndef _DEVICE_COMMUNICATION_H
#define _DEVICE_COMMUNICATION_H

#include <unistd.h>
#include <sys/signal.h>
#include "device/device.h"

#define DEVICE_COMMUNICATION_CHILD_READ 0
#define DEVICE_COMMUNICATION_CHILD_WRITE 1
#define DEVICE_COMMUNICATION_READ_PIPE SIGUSR1
#define DEVICE_COMMUNICATION_TIMER SIGUSR2
#define DEVICE_COMMUNICATION_READ_QUEUE SIGCONT
#define DEVICE_COMMUNICATION_MESSAGE_LENGTH 256
#define DEVICE_COMMUNICATION_MESSAGE_FIELDS_MAX 16
#define DEVICE_COMMUNICATION_MESSAGE_FIELDS_DELIMITER "\n"

/* Message types */
#define MESSAGE_TYPE_NO_MESSAGE 0
#define MESSAGE_TYPE_ERROR 1
#define MESSAGE_TYPE_TERMINATE 2
#define MESSAGE_TYPE_TERMINATE_CONTROLLER 3
#define MESSAGE_TYPE_INFO 4
#define MESSAGE_TYPE_SET_ON 5
#define MESSAGE_TYPE_SPAWN_DEVICE 6
#define MESSAGE_TYPE_SET_INIT_VALUES 7
#define MESSAGE_TYPE_LOCK 8
#define MESSAGE_TYPE_UNLOCK 9
#define MESSAGE_TYPE_UNLOCK_AND_DELETE 10
#define MESSAGE_TYPE_UNKNOWN 125
#define MESSAGE_TYPE_GET_PID 126
#define MESSAGE_TYPE_I_AM_ALIVE 127
#define MESSAGE_TYPE_RECIPIENT_ID_MISLEADING 128
/* END Message types */

/* Message Status */
#define MESSAGE_RETURN_SUCCESS "SUCCESS"
#define MESSAGE_RETURN_NAME_ERROR "ERROR\nNAME"
#define MESSAGE_RETURN_VALUE_ERROR "ERROR\nVALUE"
#define MESSAGE_RETURN_VALUE_PASSED_DATE_ERROR "ERROR\nPASSEDDATE"
#define MESSAGE_RETURN_VALUE_FORMAT_DATE_ERROR "ERROR\nFORMATDATE"
#define MESSAGE_RETURN_VALUE_ORDER_DATE_ERROR "ERROR\nORDERDATE"
#define MESSAGE_RETURN_VALUE_ALREADY_DEFINED_DATE_ERROR "ERROR\nALREADYDEFINED"
#define MESSAGE_RETURN_VALUE_SAME_DATE_ERROR "ERROR\nSAMEDATE"
#define MESSAGE_RETURN_VALUE_EXCEEDED_FRIDGE_ERROR "ERROR\nEXCEEDEDFRIDGE"
#define MESSAGE_RETURN_VALUE_EMPTY_FRIDGE_ERROR "ERROR\nEMPTYFRIDGE"
#define MESSAGE_RETURN_VALUE_MINTHERMO_FRIDGE_ERROR "ERROR\nMINTHERMOFRIDGE"
#define MESSAGE_RETURN_VALUE_MAXTHERMO_FRIDGE_ERROR "ERROR\nMAXTHERMOFRIDGE"

/* END Message Status */

/* Queue Message Definitions */
#define QUEUE_MESSAGE_QUEUE_NAME "/tmp/control"
#define QUEUE_MESSAGE_QUEUE_NUMBER 1

#define QUEUE_MESSAGE_TYPE_ALL_TYPES 0
#define QUEUE_MESSAGE_TYPE_DOMUS_PID_REQUEST 1
#define QUEUE_MESSAGE_TYPE_PID_REQUEST 2
#define QUEUE_MESSAGE_TYPE_DEVICE_START 5

#define QUEUE_MESSAGE_MESSAGE_LENGTH 100

#define QUEUE_MESSAGE_RETURN_SUCCESS "SUCCESS"
#define QUEUE_MESSAGE_RETURN_NAME_ERROR "ERROR_NAME"
#define QUEUE_MESSAGE_RETURN_VALUE_ERROR "ERROR_VALUE"

/* END Queue Message Definitions */
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
 * Check if a Device from A message is directly connected
 * @param message The message to check from
 * @return true if directly connected, false otherwise
 */
bool device_communication_device_is_directly_connected(const DeviceCommunicationMessage *message);

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
 * Create an exact copy of a message
 *  Remember to free!
 * @param message The message to copy
 * @return The copy of the message, NULL otherwise
 */
DeviceCommunicationMessage *device_communication_message_copy(const DeviceCommunicationMessage *message);

/**
 * Split a message into an array of fields from a message string
 *  Remember to free using device_communication_free_message_fields!
 * @param message The message to split from
 * @return An array of fields, NULL otherwise
 */
char **device_communication_split_message_fields(const char *message);

/**
 * Free the Splitted Message Fields
 * @param fields The message Fields array
 * @return true if freed, false otherwise
 */
bool device_communication_free_message_fields(char **fields);

/*
 *
 * Queue Messages
 *
 */

typedef struct Message {
    long mesg_type;
    char mesg_text[QUEUE_MESSAGE_MESSAGE_LENGTH];
} Message;

typedef struct Queue_message {
    key_t key;
    int message_id;
    Message _message;

} Queue_message;

Queue_message *new_queue_message(char queue_name[], int queue_number, int msg_type, char *msg_text, bool clear);

void queue_message_send_message(Queue_message *message);

Message *queue_message_receive_message(int msg_id, int message_type, bool blocking);

void queue_message_notify(__pid_t pid);

int queue_message_get_message_id(char queue_name[], int queue_number);

bool queue_message_remove_message_queue(int msg_id);

Message *queue_message_send_message_with_ack(__pid_t device_pid, Queue_message *message);

void queue_message_create_queue();

#endif
