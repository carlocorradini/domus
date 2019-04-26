#include "device/device.h"
#include "device/interaction/device_window.h"
#include "device/device_communication.h"
#include "device/device_child.h"
#include "util/util_converter.h"

static Device *window = NULL;
static WindowSwitch* switch_ = NULL;

static DeviceCommunication *window_communication = NULL;

bool setState(bool state){
    switch_->state = true;
    window->state=state;

    switch_->state = false;

    return window->state;
}

WindowSwitch *new_window_switch(void){
    WindowSwitch *windowSwitch = (WindowSwitch *) malloc(sizeof(WindowSwitch));

    if (windowSwitch == NULL) {
        perror("Window Switch Memory Allocation");
        exit(EXIT_FAILURE);
    }

    windowSwitch->state = false;
    windowSwitch->setFunction = setState;

    return windowSwitch;
}

WindowRegistry *new_window_registry(void){
    if (device_check_device(window)) return NULL;

    WindowRegistry *windowRegistry;
    windowRegistry = (WindowRegistry *) window->registry;

    return windowRegistry;
}

static void window_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;

    out_message.id_sender = window->id;

    switch (in_message.type) {
        case MESSAGE_TYPE_IS_ON: {
            out_message.type = MESSAGE_TYPE_IS_ON;
            snprintf(out_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "%d", window->state);
            device_communication_write_message(window_communication, &out_message);
            break;
        }
        case MESSAGE_TYPE_SET_ON: {
            out_message.type = MESSAGE_TYPE_SET_ON;
            bool tmp = converter_char_to_bool((char *) in_message.message);
            switch_->setFunction(tmp);
            snprintf(out_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "%d", window->state);
            device_communication_write_message(window_communication, &out_message);
            exit(EXIT_SUCCESS);
        }
        case MESSAGE_TYPE_TERMINATE: {
            out_message.type = MESSAGE_TYPE_TERMINATE;
            snprintf(out_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH, "%d", true);
            device_communication_write_message(window_communication, &out_message);
            exit(EXIT_SUCCESS);
        }
        default: {
            out_message.type = MESSAGE_TYPE_ERROR;
            snprintf(out_message.message, DEVICE_COMMUNICATION_MESSAGE_LENGTH,
                     "Window received something wrong: {%d, %s}",
                     in_message.type, in_message.message);
            device_communication_write_message(window_communication, &out_message);
            break;
        }
    }
}

int main(int argc, char **args){
    switch_ = new_window_switch();
    window = device_child_new_device(argc, args, new_window_registry, switch_->setFunction);
    window_communication = device_child_new_device_communication(argc, args, window_message_handler);

    while (true);
    return 0;
}
