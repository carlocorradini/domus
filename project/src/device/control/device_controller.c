
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
            ));

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

size_t controller_fork_device(const DeviceDescriptor *device_descriptor) {
    ControllerRegistry *registry;
    size_t child_id;
    if (!device_check_control_device(controller) || device_descriptor == NULL) return -1;

    child_id = ((ControllerRegistry *) controller->device->registry)->next_id++;
    if (!control_device_fork(controller, child_id, device_descriptor)) return -1;

    registry = (ControllerRegistry *) controller->device->registry;
    registry->connected_directly = registry->connected_total = controller->devices->size;

    return child_id;
}


bool controller_has_devices(void) {
    return control_device_has_devices(controller);
}

bool controller_valid_id(size_t id) {
    return control_device_valid_id(id, controller);
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
            device_communication = control_device_get_device_communication_by_id(in_message.id_sender, controller);

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
            device_communication = control_device_get_device_communication_by_id(in_message.id_sender, controller);
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
    if (!control_device_valid_id(id, controller)) return false;

    device_communication_message_init(controller->device, &out_message);
    out_message.type = MESSAGE_TYPE_TERMINATE;

    device_communication = control_device_get_device_communication_by_id(id, controller);

    controller_message_handler(device_communication_write_message_with_ack(device_communication, &out_message));

    return true;
}

bool controller_del_all(void) {
    DeviceCommunication *data;
    if (!device_check_control_device(controller)) return false;
    if (!control_device_has_devices(controller)) return false;

    list_for_each(data, controller->devices) {
        controller_del_by_id(data->id);
    }

    return true;
}

void controller_info_by_id(size_t id) {
    DeviceCommunication *device_communication;
    DeviceCommunicationMessage out_message;
    if (!device_check_control_device(controller)) return;
    if (!control_device_valid_id(id, controller)) return;

    device_communication_message_init(controller->device, &out_message);
    out_message.type = MESSAGE_TYPE_INFO;

    device_communication = control_device_get_device_communication_by_id(id, controller);
    controller_message_handler(device_communication_write_message_with_ack(device_communication, &out_message));
}

void controller_info_all(void) {
    DeviceCommunication *data;
    if (!device_check_control_device(controller)) return;
    if (!control_device_has_devices(controller)) return;

    list_for_each(data, controller->devices) {
        controller_info_by_id(data->id);
    }
}

int controller_switch(size_t id, const char *switch_label, const char *switch_pos) {
    DeviceCommunicationMessage out_message;
    DeviceCommunicationMessage in_message;
    if (!device_check_control_device(controller)) return -1;
    if (!control_device_valid_id(id, controller)) return -1;

    device_communication_message_init(controller->device, &out_message);
    device_communication_message_modify(&out_message, MESSAGE_TYPE_SET_ON,
                                        "%s\n%s\n",
                                        switch_label,
                                        switch_pos);

    in_message = device_communication_write_message_with_ack(
            control_device_get_device_communication_by_id(id, controller),
            &out_message);

    if (strcmp(in_message.message, MESSAGE_RETURN_SUCCESS) == 0) return 0;
    if (strcmp(in_message.message, MESSAGE_RETURN_NAME_ERROR) == 0) return 1;
    if (strcmp(in_message.message, MESSAGE_RETURN_VALUE_ERROR) == 0) return 2;

    return -1;
}