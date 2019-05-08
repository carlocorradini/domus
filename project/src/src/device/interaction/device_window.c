
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

    window_switch = device_get_device_switch(window->switches, name);
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
            time_t open_time = ((WindowRegistry *) window->registry)->open;
            double time_difference = (open_time == 0) ? 0.0 : difftime(time(NULL), open_time);
            bool switch_state = (bool) (device_get_device_switch_state(window->switches, "open"));

            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_INFO,
                                                "%d\n%.0lf\n%d\n",
                                                window->state, time_difference, switch_state);

            break;
        }
        case MESSAGE_TYPE_SET_INIT_VALUES: {
            ConverterResult state;
            ConverterResult open_time;
            ConverterResult switch_state;

            char **fields = device_communication_split_message_fields(&in_message);

            state = converter_char_to_bool(fields[0][0]);
            open_time = converter_string_to_long(fields[1]);
            switch_state = converter_char_to_bool(fields[2][0]);

            window->state = state.data.Bool;
            ((WindowRegistry *) window->registry)->open = time(NULL) - open_time.data.Long;

            device_get_device_switch(window->switches, "open")->state = (bool *) switch_state.data.Bool;
            
            device_communication_free_message_fields(fields);
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SET_INIT_VALUES,
                                                "");
            break;
        }
        case MESSAGE_TYPE_SET_ON: {
            out_message.type = MESSAGE_TYPE_SET_ON;
            char *switch_label;
            char *switch_pos;
            bool bool_switch_pos;

            switch_label = strtok(in_message.message, DEVICE_COMMUNICATION_MESSAGE_FIELDS_DELIMITER);
            switch_pos = strtok(NULL, DEVICE_COMMUNICATION_MESSAGE_FIELDS_DELIMITER);

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