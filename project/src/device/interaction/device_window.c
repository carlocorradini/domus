
#include <time.h>
#include <string.h>
#include "device/device_child.h"
#include "device/interaction/device_window.h"
#include "util/util_converter.h"

/**
 * The window Device
 */
static Device *window = NULL;

/**
 * The Device Communication for Window
 */
static DeviceCommunication *window_communication = NULL;

/**
 * Set the bulb switch state
 * @param name The switch name
 * @param state The state to set
 * @return true if successful, false otherwise
 */
static bool window_set_switch_state(const char *name, bool state);

/**
 * Handle the incoming message
 * @param in_message The received message
 */
static void window_message_handler(DeviceCommunicationMessage in_message);

/**
 * Check the input value if it is correct
 * @param input The input value param
 * @return true if correct, false otherwise
 */
static bool window_check_value(const char *input);

WindowRegistry *new_window_registry(void) {
    WindowRegistry *window_registry;
    if (window != NULL) return NULL;

    window_registry = (WindowRegistry *) malloc(sizeof(WindowRegistry));
    if (window_registry == NULL) {
        perror("Window Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }

    window_registry->open = time(NULL);

    return window_registry;
}

static bool window_set_switch_state(const char *name, bool state) {
    WindowRegistry *window_registry;
    DeviceSwitch *window_switch;
    if (!list_contains(window->switches, name)) return false;

    if (window->state == state) return true;

    window_switch = list_get(window->switches, list_get_index(window->switches, name));
    window_registry = (WindowRegistry *) window->registry;

    window_switch->state = (bool *) true;
    window->state = state;
    window_registry->open = (state) ? time(NULL) : (time_t) 0;
    window_switch->state = (bool *) false;

    return true;
}

static bool window_check_value(const char *input) {
    return strcmp(input, "on") == 0 || strcmp(input, "off") == 0;
}

static void window_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    ConverterResult result;

    device_communication_message_init(window, &out_message);

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            device_communication_message_modify(&out_message, MESSAGE_TYPE_INFO,
                                                "%ld     %s     %s",
                                                window->id,
                                                "windowww",
                                                "semplice window");
            break;
        }
        case MESSAGE_TYPE_SET_ON: {
            out_message.type = MESSAGE_TYPE_SET_ON;
            char *switch_label;
            char *switch_pos;
            bool bool_switch_pos;

            switch_label = strtok(in_message.message, MESSAGE_DELIMITER);
            switch_pos = strtok(NULL, MESSAGE_DELIMITER);

            if (!window_check_value(switch_pos)) {
                device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_ERROR);
                break;
            }

            bool_switch_pos = strcmp(switch_pos, "on") == 0 ? true : false;

            window_set_switch_state(switch_label, bool_switch_pos)
            ? device_communication_message_modify_message(&out_message, MESSAGE_RETURN_SUCCESS)
            : device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);

            break;
        }
        default: {
            device_communication_message_modify(&out_message, MESSAGE_TYPE_ERROR,
                                                "{%d, %s}",
                                                in_message.type,
                                                in_message.message);
            break;
        }
    }

    device_communication_write_message(window_communication, &out_message);
}

int main(int argc, char **args) {
    window = device_child_new_device(argc, args, new_window_registry());
    list_add_last(window->switches, new_device_switch("open", false, window_set_switch_state));

    window_communication = device_child_new_device_communication(argc, args, window_message_handler);

    device_child_run(NULL);

    return EXIT_SUCCESS;
}