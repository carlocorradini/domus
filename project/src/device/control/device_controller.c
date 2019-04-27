
#include <string.h>
#include <sys/wait.h>
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
 * Methods to compare DeviceCommunication and id
 * @param data1 DeviceCommunication data (element of controller->devices)
 * @param data2 id
 * @return true if  equals, false otherwise
 */
static bool process_equals(const DeviceCommunication *data1, const size_t *data2) {
    return data1->id == (size_t) data2;
}

/**
 * Print information about a Device given a Device Communication
 * @param device_communication The Device Communication
 */
static void controller_info_print(const DeviceCommunication *device_communication);

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
                       new_controller_registry()
            ),
            new_list(NULL, process_equals));

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

static void controller_message_handler(DeviceCommunicationMessage in_message) {
    switch (in_message.type) {
        case MESSAGE_TYPE_DEBUG: {
            println("\tDEBUG MESSAGE");
            println("\tMessage from %ld: %s", in_message.id_sender, in_message.message);
            break;
        }
        case MESSAGE_TYPE_ERROR: {
            println_color(COLOR_RED, "\tERROR MESSAGE");
            println("\tMessage from %ld: %s", in_message.id_sender, in_message.message);
            break;
        }
        case MESSAGE_TYPE_INFO: {
            println("\t%s", in_message.message);
            break;
        }
        case MESSAGE_TYPE_TERMINATE: {
            DeviceCommunication *data;
            size_t index = 0;
            list_for_each(data, controller->devices) {
                if (data->id == in_message.id_sender) {
                    close(data->com_read);
                    close(data->com_write);
                    waitpid(data->pid, 0, 0);
                    list_remove_index(controller->devices, index);
                }
                index++;
            }
            println("\tDevice %ld has been terminated with status {%s}", in_message.id_sender, in_message.message);
            break;
        }
        default: {
            println_color(COLOR_RED, "\tMessage type [%d] | [%s] not supported", in_message.type, in_message.message);
            break;
        }
    }
}

bool controller_has_devices(void) {
    if (!device_check_control_device(controller)) return false;
    return !list_is_empty(controller->devices);
}

void controller_list(void) {
    DeviceCommunication *data;
    DeviceCommunicationMessage out_message;
    if (!device_check_control_device(controller)) return;

    out_message.type = MESSAGE_TYPE_INFO;
    out_message.id_sender = 0;

    list_for_each(data, controller->devices) {
        controller_message_handler(device_communication_write_message_with_ack(data, &out_message));
    }
}

bool controller_del(size_t id) {
    DeviceCommunication *data;
    DeviceCommunicationMessage message;
    if (!device_check_control_device(controller)) return false;

    message.type = MESSAGE_TYPE_TERMINATE;
    message.id_sender = 0;

    list_for_each(data, controller->devices) {
        controller_message_handler(device_communication_write_message_with_ack(data, &message));
    }

    return false;
}

size_t controller_valid_id(size_t id) {
    if (!device_check_control_device(controller)) return -1;

    if (id <= 0) return -1;

    if (list_contains(controller->devices, (size_t *) id)) {
        return id;
    }

    return -1;
}

void controller_info_all(void) {
    DeviceCommunication *data;
    if (!device_check_control_device(controller)) return;

    if (controller_has_devices()) {
        list_for_each(data, controller->devices) {
            controller_info_by_id(data->id);
        }
    }
}

void controller_info_by_id(size_t id) {
    DeviceCommunication *device_communication;
    DeviceCommunicationMessage out_message;
    if (!device_check_control_device(controller)) return;

    out_message.type = MESSAGE_TYPE_INFO;
    out_message.id_sender = 0;

    if (list_contains(controller->devices, (size_t *) id)) {
        device_communication = (DeviceCommunication *) list_get(controller->devices,
                                                                (size_t) list_get_index(controller->devices,
                                                                                        (size_t *) id));
        controller_message_handler(device_communication_write_message_with_ack(device_communication, &out_message));
    }
}

bool set_device_switch(size_t id, char switch_name[], char switch_value[]){
    DeviceCommunication *data;
    DeviceCommunicationMessage out_message;
    DeviceCommunicationMessage in_message;
    if (!device_check_control_device(controller)) return false;
    if (controller_valid_id(id) == -1) return false;

    out_message.type = MESSAGE_TYPE_SET_ON;
    out_message.id_sender = 0;
    snprintf(out_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "%s\n%s\n", switch_name, switch_value);

    data = list_get(controller->devices, list_get_index(controller->devices, id));

    in_message = device_communication_write_message_with_ack(data, &out_message);

    return strcmp(in_message.message, "Success") == 0;
}