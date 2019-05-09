
#include "device/device.h"
#include "device/device_child.h"
#include "device/device_communication.h"
#include "device/control/device_controller.h"

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

    controller_registry->directly_connected_child = 0;

    return controller_registry;
}

static void controller_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    device_communication_message_init(controller->device, &out_message);

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_INFO,
                                                "%ld\n%ld\n",
                                                controller->devices->size,
                                                ((ControllerRegistry *) controller->device->registry)->directly_connected_child);
            break;
        }
        case MESSAGE_TYPE_SPAWN_DEVICE: {
            device_child_set_device_to_spawn(in_message);
            ((ControllerRegistry *) controller->device->registry)->directly_connected_child++;
            return;
        }
        default: {
            device_communication_message_modify(&out_message, MESSAGE_TYPE_ERROR,
                                                "{%d, %s}",
                                                in_message.type,
                                                in_message.message);
            break;
        }
    }

    device_communication_write_message(controller_communication, &out_message);
}

int main(int argc, char **args) {
    controller = device_child_new_control_device(argc, args, new_controller_registry());
    controller_communication = device_child_new_control_device_communication(argc, args, controller_message_handler);

    device_child_run(NULL);

    return EXIT_SUCCESS;
}