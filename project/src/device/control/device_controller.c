
#include <fcntl.h>
#include <string.h>
#include "device/control/device_controller.h"
#include "device/device_communication.h"
#include "device/device_child.h"
#include "cli/command/command.h"
#include "util/util_printer.h"
#include "util/util_converter.h"
#include "cli/cli.h"
#include "author.h"

/**
 * Controller that must be defined and it cannot be visible outside this file
 * Only one can exist in the entire program!!!
 */
static ControlDevice *controller = NULL;

/**
 * Initialize all Controller Components
 */
static void controller_init(void);

/**
 * Free all Controller Components
 */
static void controller_tini(void);

/**
 * Replaces the current running process with a new device process described in the Device Descriptor
 * @param child_id The child id
 * @param parent_id The parent id
 * @param device_descriptor The descriptor of the device to be created
 */
static void controller_fork_child(size_t child_id, size_t parent_id, const DeviceDescriptor *device_descriptor);

/**
 * Add the new child process to the controller devices list for communication
 * @param child_id The id of the child process
 * @param pid The pid of the child process
 * @param device_descriptor The Device Descriptor of the new process
 * @param com_read File Descriptor for parent -> READ
 * @param com_write File Descriptor for parent -> WRITE
 */
static void controller_fork_parent(size_t child_id, pid_t pid, const DeviceDescriptor *device_descriptor, int com_read,
                                   int com_write);

/**
 * Change the state of the controller
 * @param state The state to change to
 * @return true if the operation was successful, false otherwise
 */
static bool controller_master_switch(bool state);

/**
 * Wake Up the controller to read the read pipes
 * @param signal_number DEVICE_COMMUNICATION_READ_PIPE signal for reading
 */
static void controller_read_pipe(int signal_number);

/**
 * Handle incoming message
 * @param message The message to handle
 */
static void controller_message_handler(DeviceCommunicationMessage message);

void controller_start(void) {
    controller_init();
    cli_start();
    controller_tini();
}

static void controller_init(void) {
    /* Create the controller, only once in the entire program with id 0 */
    controller = new_control_device(
            new_device(getpid(),
                       0,
                       DEVICE_STATE,
                       new_controller_registry(),
                       controller_master_switch),
            new_list(NULL, NULL));
    /* Attach a macro to DEVICE_COMMUNICATION_READ_PIPE the signal to force the controller to check new messages */
    signal(DEVICE_COMMUNICATION_READ_PIPE, controller_read_pipe);

    command_init();
    author_init();
    device_init();
}

static void controller_tini(void) {
    free_control_device(controller);
    command_tini();
    author_tini();
    device_tini();
}

static bool controller_master_switch(bool state) {
    if (!device_check_control_device(controller)) return false;
    controller->device->state = state;
    return true;
}

ControllerRegistry *new_controller_registry(void) {
    ControllerRegistry *controller_registry;
    if (device_check_control_device(controller)) return NULL;

    controller_registry = (ControllerRegistry *) malloc(sizeof(ControllerRegistry));
    if (controller_registry == NULL) {
        perror("Controller Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }

    controller_registry->next_id = 1;
    controller_registry->connected_directly = controller_registry->connected_total = 0;

    return controller_registry;
}

bool controller_fork_device(const DeviceDescriptor *device_descriptor) {
    pid_t pid_child;
    size_t child_id;
    size_t parent_id;
    int write_parent_read_child[2];
    int write_child_read_parent[2];
    if (!device_check_control_device(controller) || device_descriptor == NULL) return false;

    if (pipe(write_parent_read_child) == -1
        || pipe(write_child_read_parent) == -1) {
        perror("Controller Fork Pipe");
        exit(EXIT_FAILURE);
    }
    /* Async Pipe Reading */
    if (fcntl(write_parent_read_child[0], F_SETFL, O_NONBLOCK) == -1
        || fcntl(write_child_read_parent[0], F_SETFL, O_NONBLOCK) == -1) {
        perror("Controller Fork fcntl");
        exit(EXIT_FAILURE);
    }

    /* Get next valid id and after increment by one */
    child_id = ((ControllerRegistry *) controller->device->registry)->next_id++;
    /* Get parent id */
    parent_id = controller->device->id;
    /* Fork the current process */
    switch (pid_child = fork()) {
        case -1: {
            perror("Controller Fork Forking");
            exit(EXIT_FAILURE);
        }
        case 0: {
            close(write_parent_read_child[1]);
            close(write_child_read_parent[0]);

            /* Attach child stdout to write child pipe */
            dup2(write_child_read_parent[1], DEVICE_COMMUNICATION_CHILD_WRITE);
            /* Attach child stdin to read child pipe */
            dup2(write_parent_read_child[0], DEVICE_COMMUNICATION_CHILD_READ);

            controller_fork_child(child_id, parent_id, device_descriptor);
            break;
        }
        default: {
            close(write_parent_read_child[0]);
            close(write_child_read_parent[1]);

            controller_fork_parent(child_id, pid_child, device_descriptor, write_child_read_parent[0],
                                   write_parent_read_child[1]);
            break;
        }
    }

    return true;
}

static void controller_fork_child(size_t child_id, size_t parent_id, const DeviceDescriptor *device_descriptor) {
    char *device_args[DEVICE_CHILD_ARGS_LENGTH + 1];
    char device_id[sizeof(size_t) + 1];
    char device_parent_id[sizeof(size_t) + 1];
    char device_name[DEVICE_NAME_LENGTH];
    char device_description[DEVICE_DESCRIPTION_LENGTH];
    if (device_descriptor == NULL) return;

    snprintf(device_id, sizeof(size_t) + 1, "%ld", child_id);
    snprintf(device_parent_id, sizeof(size_t) + 1, "%ld", parent_id);
    strncpy(device_name, device_descriptor->name, DEVICE_NAME_LENGTH);
    strncpy(device_description, device_descriptor->description, DEVICE_DESCRIPTION_LENGTH);

    device_args[0] = device_id;
    device_args[1] = device_parent_id;
    device_args[2] = device_name;
    device_args[3] = device_description;
    device_args[4] = NULL;

    if (execv(device_descriptor->file_name, device_args) == -1) {
        perror("Error exec Controller Fork Child");
        exit(EXIT_FAILURE);
    }
}

static void controller_fork_parent(size_t child_id, pid_t pid, const DeviceDescriptor *device_descriptor, int com_read,
                                   int com_write) {
    ControllerRegistry *registry;
    if (!device_check_control_device(controller)) return;
    if (pid < 0 || com_read < 0 || com_write < 0) return;

    list_add_last(controller->devices, new_device_communication(child_id, pid, device_descriptor, com_read, com_write));
    registry = (ControllerRegistry *) controller->device->registry;
    registry->connected_directly = registry->connected_total = controller->devices->size;
}

static void controller_message_handler(DeviceCommunicationMessage message) {
    switch (message.type) {
        case MESSAGE_TYPE_DEBUG: {
            println("\tDEBUG MESSAGE");
            println("\tMessage from %ld: %s", message.id_sender, message.message);
            break;
        }
        case MESSAGE_TYPE_ERROR: {
            println_color(COLOR_RED, "\tERROR MESSAGE");
            println("\tMessage from %ld: %s", message.id_sender, message.message);
            break;
        }
        case MESSAGE_TYPE_TERMINATE: {
            DeviceCommunication *data;
            size_t index = 0;
            list_for_each(data, controller->devices) {
                if (data->id == message.id_sender) {
                    list_remove_index(controller->devices, index);
                }
                index++;
            }
            println("\tDevice %ld has been terminated with status {%s}", message.id_sender, message.message);
            break;
        }
        case MESSAGE_TYPE_IS_ON: {
            println("\tDevice %ld is %s", message.id_sender, message.message);
            break;
        }
        case MESSAGE_TYPE_SET_ON: {
            println("\tDevice %ld has been changed", message.id_sender);
            break;
        }
        default: {
            println_color(COLOR_RED, "\tMessage type [%d] | [%s] not supported", message.type, message.message);
            break;
        }
    }
}

static void controller_read_pipe(int signal_number) {
    if (signal_number == DEVICE_COMMUNICATION_READ_PIPE) {
        DeviceCommunication *data;
        if (!device_check_control_device(controller)) return;

        list_for_each(data, controller->devices) {
            device_communication_read_message(data, controller_message_handler);
        }
    }
}

void controller_list(void) {
    DeviceCommunication *data;
    DeviceCommunicationMessage message;
    if (!device_check_control_device(controller)) return;

    message.type = MESSAGE_TYPE_IS_ON;
    message.id_sender = 0;

    list_for_each(data, controller->devices) {
        device_communication_write_message(data, &message);
    }
}

bool controller_del(size_t id) {
    DeviceCommunication *data;
    DeviceCommunicationMessage message;
    if (!device_check_control_device(controller)) return false;

    message.type = MESSAGE_TYPE_TERMINATE;
    message.id_sender = 0;

    list_for_each(data, controller->devices) {
        device_communication_write_message(data, &message);
    }

    return false;
}

size_t controller_valid_id(size_t id) {
    DeviceCommunication *data;
    if (id <= 0) return -1;

    list_for_each(data, controller->devices) {
        if (data->id == id) {
            return id;
        }
    }

    return -1;
}