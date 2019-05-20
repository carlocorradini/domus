
#include "device/device.h"
#include "device/device_child.h"
#include "device/device_communication.h"
#include "device/control/device_controller.h"
#include <string.h>
#include "util/util_converter.h"
/**
 * Controller that must be defined and it cannot be visible outside this file
 * Only one can exist in the entire program!!!
 */
static ControlDevice *controller = NULL;

/**
 * The Device Communication for Controller
 */
static DeviceCommunication *controller_communication = NULL;

/**
 * Handle the incoming message
 * @param in_message The received message
 */
static void controller_message_handler(DeviceCommunicationMessage in_message);

ControllerRegistry *new_controller_registry(void) {
    ControllerRegistry *controller_registry;
    if (device_check_control_device(controller)) return NULL;

    controller_registry = (ControllerRegistry *) malloc(sizeof(ControllerRegistry));
    if (controller_registry == NULL) {
        perror("Controller Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }
    controller_registry->directly_connected_devices = 0;

    return controller_registry;
}

static void controller_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    device_communication_message_init(controller->device, &out_message);
    ((ControllerRegistry *) controller->device->registry)->directly_connected_devices = controller->devices->size;
    out_message.override = in_message.override;

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_INFO,
                                                "%d\n%ld\n",
                                                controller->device->state,
                                                ((ControllerRegistry *) controller->device->registry)->directly_connected_devices);
            break;
        }
        case MESSAGE_TYPE_SPAWN_DEVICE: {
            device_child_set_device_to_spawn(in_message);
            return;
        }
        default: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_UNKNOWN, "%s",
                                                in_message.message);
            break;
        }
    }

    device_communication_write_message(controller_communication, &out_message);
}

static int controller_set_switch_state(const char *name, bool state) {
    if (strcmp(name, CONTROLLER_SWITCH_STATE) == 0) {
        DeviceSwitch *controller_switch = device_get_device_switch(controller->device->switches, name);
        if (state) {
            //controller_switch->state = (void *) 0;
            return 1;
        } else{
            //controller_switch->state = (void *) 1;
            return 1;
        }
        return -1;
    }
    return false;
}

static void queue_message_handler() {
    Message *in_message;
    Queue_message *out_message;
    ConverterResult sender_pid;
    char text[QUEUE_MESSAGE_MESSAGE_LENGTH];
    char **fields;
    int message_id;

    message_id = queue_message_get_message_id(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER);
    in_message = queue_message_receive_message(message_id, QUEUE_MESSAGE_TYPE_DEVICE_START + controller->device->id, true);

    fields = device_communication_split_message_fields(in_message->mesg_text);

    sender_pid = converter_string_to_long(fields[0]);
    snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_CONTROLLER, QUEUE_MESSAGE_RETURN_NAME_ERROR);

    fprintf(stderr, "\n\n%s\n\n", in_message->mesg_text);
    if (strcmp(fields[1], CONTROLLER_SWITCH_STATE) == 0) {
        snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_CONTROLLER, QUEUE_MESSAGE_RETURN_VALUE_ERROR);
        if (strcmp(fields[2], CONTROLLER_SWITCH_STATE_OFF) == 0) {
            if (controller_set_switch_state(CONTROLLER_SWITCH_STATE, false)) {
                snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_CONTROLLER, QUEUE_MESSAGE_RETURN_SUCCESS);
                controller->device->override = true;
            }
        } else if (strcmp(fields[2], CONTROLLER_SWITCH_STATE_ON) == 0) {
            if (controller_set_switch_state(CONTROLLER_SWITCH_STATE, true)) {
                snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_CONTROLLER, QUEUE_MESSAGE_RETURN_SUCCESS);
                controller->device->override = true;
            }
        }
    }

    out_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER,
                                    QUEUE_MESSAGE_TYPE_DEVICE_START + controller->device->id, text, false);
    queue_message_send_message(out_message);
    queue_message_notify((__pid_t) sender_pid.data.Long);

    device_communication_free_message_fields(fields);
}

int main(int argc, char **args) {
    controller = device_child_new_control_device(argc, args, DEVICE_TYPE_CONTROLLER, new_controller_registry());

    list_add_last(controller->device->switches,
                  new_device_switch(CONTROLLER_SWITCH_STATE, (bool *) DEVICE_STATE,
                                    (int (*)(const char *, void *)) controller_set_switch_state));

    controller_communication = device_child_new_control_device_communication(argc, args, controller_message_handler);

    signal(DEVICE_COMMUNICATION_READ_QUEUE, queue_message_handler);
    device_child_run(NULL);

    return EXIT_SUCCESS;
}