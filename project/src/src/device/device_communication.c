
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/msg.h>
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
new_device_communication(pid_t pid, int com_read, int com_write) {
    DeviceCommunication *device_communication = (DeviceCommunication *) malloc(sizeof(DeviceCommunication));
    if (device_communication == NULL) {
        perror("Device Communication Memory Allocation");
        exit(EXIT_FAILURE);
    }

    device_communication->pid = pid;
    device_communication->com_read = com_read;
    device_communication->com_write = com_write;

    return device_communication;
}

bool device_communication_close_communication(DeviceCommunication *device_communication) {
    if (device_communication == NULL) return false;

    if (close(device_communication->com_read) == -1
        || close(device_communication->com_write) == -1 ||
        waitpid(device_communication->pid, 0, 0) == -1) {
        perror("Error closing pipe in Read Message");
        exit(EXIT_FAILURE);
    }

    return true;
}

bool device_communication_device_is_directly_connected(const DeviceCommunicationMessage *message) {
    if (message == NULL) return false;
    return message->ctr_hop == 1;
}

DeviceCommunicationMessage device_communication_read_message(DeviceCommunication *device_communication) {
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
            device_communication_close_communication(device_communication);

            break;
        }
        default: {
            /* Message was correctly received */
            break;
        }
    }

    in_message.ctr_hop++;
    return in_message;
}

DeviceCommunicationMessage device_communication_write_message_with_ack(DeviceCommunication *device_communication,
                                                                       const DeviceCommunicationMessage *out_message) {
    DeviceCommunicationMessage in_message;
    if (device_communication == NULL || out_message == NULL) {
        device_communication_message_modify(&in_message, 0, MESSAGE_TYPE_ERROR,
                                            "Device Communication OR Message has not been initialized");
        return in_message;
    }

    device_communication_write_message(device_communication, out_message);

    device_communication_notify(device_communication->pid);

    return device_communication_read_message(device_communication);
}

DeviceCommunicationMessage device_communication_write_message_with_ack_silent(DeviceCommunication *device_communication,
                                                                              const DeviceCommunicationMessage *out_message) {
    DeviceCommunicationMessage in_message;
    if (device_communication == NULL || out_message == NULL) {
        device_communication_message_modify(&in_message, 0, MESSAGE_TYPE_ERROR,
                                            "Device Communication OR Message has not been initialized");
        return in_message;
    }

    device_communication_write_message(device_communication, out_message);

    return device_communication_read_message(device_communication);
}

void device_communication_write_message(const DeviceCommunication *device_communication,
                                        const DeviceCommunicationMessage *out_message) {
    if (device_communication == NULL || out_message == NULL) return;

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

    message->type = MESSAGE_TYPE_ERROR;
    message->ctr_hop = 0;
    message->id_sender = device->id;
    message->id_device_descriptor = device->device_descriptor->id;
    message->flag_force = false;
    message->flag_continue = false;
    message->override = false;
    strncpy(message->device_name, device->name, DEVICE_NAME_LENGTH);

    snprintf(message->message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "Message has not been initialized");
}

static void
_device_communication_modify_message(DeviceCommunicationMessage *message, const char *message_message, va_list args) {
    if (message == NULL || message_message == NULL) return;

    vsnprintf(message->message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, message_message, args);
}

void device_communication_message_modify(DeviceCommunicationMessage *message, size_t id_recipient, size_t message_type,
                                         const char *message_message, ...) {
    if (message == NULL || message_message == NULL) return;

    message->id_recipient = id_recipient;
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

DeviceCommunicationMessage *device_communication_message_copy(const DeviceCommunicationMessage *message) {
    DeviceCommunicationMessage *message_copy;
    if (message == NULL) return NULL;

    message_copy = (DeviceCommunicationMessage *) malloc(sizeof(DeviceCommunicationMessage));
    if (message_copy == NULL) {
        perror("Device Communication Message Copy Memory Allocation");
        exit(EXIT_FAILURE);
    }

    message_copy->type = message->type;
    message_copy->ctr_hop = message->ctr_hop;
    message_copy->id_sender = message->id_sender;
    message_copy->id_recipient = message->id_recipient;
    message_copy->id_device_descriptor = message->id_device_descriptor;
    message_copy->flag_force = message->flag_force;
    message_copy->flag_continue = message->flag_continue;
    message_copy->override = message->override;
    strncpy(message_copy->message, message->message, DEVICE_COMMUNICATION_MESSAGE_LENGTH);
    strncpy(message_copy->device_name, message->device_name, DEVICE_NAME_LENGTH);

    return message_copy;
}

char **device_communication_split_message_fields(const char *message) {
    char message_copy[DEVICE_COMMUNICATION_MESSAGE_LENGTH];
    int position = 0;
    int buffer_size = DEVICE_COMMUNICATION_MESSAGE_FIELDS_MAX + 1;
    char *token;
    char **buffer;
    if (message == NULL || strlen(message) == 0) return NULL;

    buffer = (char **) malloc(sizeof(char *) * buffer_size);
    if (buffer == NULL) {
        perror("Device Communication Message Fields Buffer Memory Allocation");
        exit(EXIT_FAILURE);
    }

    strncpy(message_copy, message, DEVICE_COMMUNICATION_MESSAGE_LENGTH);

    token = strtok((char *) message_copy, DEVICE_COMMUNICATION_MESSAGE_FIELDS_DELIMITER);
    while (token != NULL) {
        /* Buffer dimension reached */
        if (position >= DEVICE_COMMUNICATION_MESSAGE_FIELDS_MAX) {
            fprintf(stderr, "Device Communication Message Fields Maximum Number of Fields Reached\n");
            break;
        }

        buffer[position] = (char *) malloc(sizeof(char) * DEVICE_COMMUNICATION_MESSAGE_LENGTH);
        if (buffer[position] == NULL) {
            perror("Device Communication Message Fields String Memory Allocation");
            exit(EXIT_FAILURE);
        }
        strncpy(buffer[position], token, DEVICE_COMMUNICATION_MESSAGE_LENGTH);
        position++;
        token = strtok(NULL, DEVICE_COMMUNICATION_MESSAGE_FIELDS_DELIMITER);
    }

    buffer[position] = NULL;
    return buffer;
}

bool device_communication_free_message_fields(char **fields) {
    size_t i;
    if (fields == NULL) return false;

    i = 0;
    while (fields[i] != NULL) {
        free(fields[i]);
        ++i;
    }

    free(fields);

    return true;
}

Queue_message *new_queue_message(char queue_name[], int queue_number, int msg_type, char *msg_text, bool clear) {
    Queue_message *tmp_message;

    tmp_message = malloc(sizeof(Queue_message));

    if (tmp_message == NULL) {
        perror("Queue_message Memory Allocation Error");
        exit(EXIT_FAILURE);
    }
    tmp_message->key = ftok(queue_name, queue_number);
    tmp_message->message_id = msgget(tmp_message->key, 0666 | IPC_CREAT);

    if (clear) {
        queue_message_remove_message_queue(tmp_message->message_id);
        tmp_message->message_id = msgget(tmp_message->key, 0666 | IPC_CREAT);
    }

    tmp_message->_message.mesg_type = msg_type;

    strncpy(tmp_message->_message.mesg_text, msg_text, QUEUE_MESSAGE_MESSAGE_LENGTH);

    return tmp_message;
}

void queue_message_send_message(Queue_message *message) {
    msgsnd(message->message_id, &message->_message, sizeof(Message), 0);
}

Message *queue_message_receive_message(int msg_id, int message_type, bool blocking) {
    Message *tmp_message;
    tmp_message = malloc(sizeof(Message));

    if (tmp_message == NULL) {
        perror("Message Memory Allocation Error");
        exit(EXIT_FAILURE);
    }

    if (!blocking) {
        msgrcv(msg_id, tmp_message, sizeof(Message), message_type, 0);
    } else {
        msgrcv(msg_id, tmp_message, sizeof(Message), message_type, IPC_NOWAIT);
    }

    return tmp_message;
}

void queue_message_notify(__pid_t pid) {
    kill(pid, DEVICE_COMMUNICATION_READ_QUEUE);
}

int queue_message_get_message_id(char queue_name[], int queue_number) {
    return msgget(ftok(queue_name, queue_number), 0666 | IPC_CREAT);
}

bool queue_message_remove_message_queue(int msg_id) {
    if (msgctl(msg_id, IPC_RMID, NULL) == 0) {
        return true;
    }
    return false;
}

static void queue_message_useless_handler() {}

Message *queue_message_send_message_with_ack(__pid_t device_pid, Queue_message *message) {
    Message *in_message;
    queue_message_send_message(message);

    signal(DEVICE_COMMUNICATION_READ_QUEUE, queue_message_useless_handler);

    queue_message_notify(device_pid);
    pause();

    in_message = queue_message_receive_message(message->message_id, message->_message.mesg_type, true);

    if (!queue_message_remove_message_queue(message->message_id)) {
        fprintf(stderr, "\tError while closing queue message queue\n");
        return NULL;
    }
    return in_message;
}

void queue_message_create_queue() {
    char create_command[256];
    snprintf(create_command, sizeof(create_command), "touch %s", QUEUE_MESSAGE_QUEUE_NAME);
    system(create_command);
}