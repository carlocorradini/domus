
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
static bool process_equals(const DeviceCommunication *data_1, const DeviceCommunication *data_2);

/**
 * Return a DeviceCommunication given an id
 * @param id The Device id
 * @return The DeviceCommunication, NULL otherwise
 */
static DeviceCommunication *controller_get_device_communication_by_id(size_t id);

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

static bool process_equals(const DeviceCommunication *data_1, const DeviceCommunication *data_2) {
    return data_1->id == data_2->id;
}

static void controller_message_handler(DeviceCommunicationMessage in_message) {
    ConverterResult result;
    DeviceCommunication *device_communication;

    switch (in_message.type) {
        case MESSAGE_TYPE_DEBUG: {
            println_color(COLOR_CYAN, "\tDEBUG MESSAGE");
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
            device_communication = controller_get_device_communication_by_id(in_message.id_sender);

            close(device_communication->com_read);
            close(device_communication->com_write);
            waitpid(device_communication->pid, 0, 0);

            result = converter_bool_to_string(converter_char_to_bool(in_message.message[0]).data.Bool);

            println_color(COLOR_GREEN,
                          "\t%s with id %ld & pid %d has been deleted with status %s",
                          device_communication->device_descriptor->name,
                          device_communication->id,
                          device_communication->pid,
                          result.data.String);

            list_remove(controller->devices, device_communication);

            break;
        }
        default: {
            device_communication = controller_get_device_communication_by_id(in_message.id_sender);
            println_color(COLOR_RED, "\tUNKNOWN MESSAGE");
            println("\tFrom %ld with pid %d: {%d, %s}",
                    device_communication->id,
                    device_communication->pid,
                    in_message.type,
                    in_message.message);
            break;
        }
    }
}

bool controller_has_devices(void) {
    if (!device_check_control_device(controller)) return false;
    return !list_is_empty(controller->devices);
}

bool controller_valid_id(size_t id) {
    DeviceCommunication fake_communication;
    if (!controller_has_devices()) return false;
    if (id <= 0) return false;

    fake_communication.id = id;

    return list_contains(controller->devices, &fake_communication);
}

static DeviceCommunication *controller_get_device_communication_by_id(size_t id) {
    DeviceCommunication fake_communication;
    if (!controller_valid_id(id)) return NULL;

    fake_communication.id = id;

    return (DeviceCommunication *) list_get(controller->devices,
                                            list_get_index(controller->devices, &fake_communication));
}

void controller_list(void) {
    DeviceCommunication *data;
    DeviceCommunicationMessage out_message;
    if (!device_check_control_device(controller)) return;

    device_communication_message_init(controller->device, &out_message);
    out_message.type = MESSAGE_TYPE_INFO;

    list_for_each(data, controller->devices) {
        controller_message_handler(device_communication_write_message_with_ack(data, &out_message));
    }
}

bool controller_del_by_id(size_t id) {
    DeviceCommunication *device_communication;
    DeviceCommunicationMessage out_message;
    if (!device_check_control_device(controller)) return false;
    if (!controller_valid_id(id)) return false;

    device_communication_message_init(controller->device, &out_message);
    out_message.type = MESSAGE_TYPE_TERMINATE;

    device_communication = controller_get_device_communication_by_id(id);

    controller_message_handler(device_communication_write_message_with_ack(device_communication, &out_message));

    return true;
}

bool controller_del_all(void) {
    DeviceCommunication *data;
    if (!device_check_control_device(controller)) return false;
    if (!controller_has_devices()) return false;

    list_for_each(data, controller->devices) {
        controller_del_by_id(data->id);
    }

    return true;
}

void controller_command_info_by_id(size_t id) {
    DeviceCommunication *device_communication;
    DeviceCommunicationMessage out_message;
    if (!device_check_control_device(controller)) return;
    if (!controller_valid_id(id)) return;

    device_communication_message_init(controller->device, &out_message);
    out_message.type = MESSAGE_TYPE_INFO;

    device_communication = controller_get_device_communication_by_id(id);
    controller_message_handler(device_communication_write_message_with_ack(device_communication, &out_message));
}

void controller_info_all(void) {
    DeviceCommunication *data;
    if (!device_check_control_device(controller)) return;

    if (controller_has_devices()) {
        list_for_each(data, controller->devices) {
            controller_command_info_by_id(data->id);
        }
    }
}

int controller_switch(size_t id, char *switch_label, char *switch_pos) {
    DeviceCommunicationMessage out_message;
    DeviceCommunicationMessage in_message;
    if (!device_check_control_device(controller)) return -1;
    if (!controller_valid_id(id)) return -1;

    device_communication_message_init(controller->device, &out_message);
    device_communication_message_modify(&out_message, MESSAGE_TYPE_SET_ON,
                                        "%s\n%s\n",
                                        switch_label,
                                        switch_pos);

    in_message = device_communication_write_message_with_ack(controller_get_device_communication_by_id(id),
                                                             &out_message);

    if (strcmp(in_message.message, MESSAGE_RETURN_SUCCESS) == 0) return 0;
    if (strcmp(in_message.message, MESSAGE_RETURN_NAME_ERROR) == 0) return 1;
    if (strcmp(in_message.message, MESSAGE_RETURN_VALUE_ERROR) == 0) return 2;

    return -1;
}