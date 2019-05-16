
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

/**
 * The queue_message_handler, it handles the incoming
 * queue messages and send them back
 */
static void queue_message_handler();

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
    return strcmp(input, WINDOW_SWITCH_OPEN_ON) == 0 || strcmp(input, WINDOW_SWITCH_OPEN_OFF) == 0;
}

static void window_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    device_communication_message_init(window, &out_message);

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            time_t open_time = ((WindowRegistry *) window->registry)->open;
            double time_difference = (open_time == 0) ? 0.0 : difftime(time(NULL), open_time);
            bool switch_state = (bool) (device_get_device_switch_state(window->switches, WINDOW_SWITCH_OPEN));

            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_INFO,
                                                "%d\n%.0lf\n%d\n",
                                                window->state, time_difference, switch_state);

            break;
        }
        case MESSAGE_TYPE_SET_INIT_VALUES: {
            ConverterResult state;
            ConverterResult open_time;
            ConverterResult switch_state;
            char **fields;

            fields = device_communication_split_message_fields(in_message.message);

            state = converter_char_to_bool(fields[2][0]);
            open_time = converter_string_to_long(fields[3]);
            switch_state = converter_char_to_bool(fields[4][0]);

            window->state = state.data.Bool;
            ((WindowRegistry *) window->registry)->open = time(NULL) - open_time.data.Long;

            device_get_device_switch(window->switches, WINDOW_SWITCH_OPEN)->state = (bool *) switch_state.data.Bool;

            device_communication_free_message_fields(fields);
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SET_INIT_VALUES,
                                                "");
            break;
        }
        case MESSAGE_TYPE_SWITCH: {
            char **fields;

            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SWITCH, "");
            fields = device_communication_split_message_fields(in_message.message);

            if (device_get_device_switch(window->switches, fields[0]) == NULL) {
                device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);
            } else if (!window_check_value(fields[1])) {
                device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_ERROR);
            } else {
                window_set_switch_state(fields[0], strcmp(fields[1], WINDOW_SWITCH_OPEN_ON) == 0)
                ? device_communication_message_modify_message(&out_message, MESSAGE_RETURN_SUCCESS)
                : device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);
            }

            device_communication_free_message_fields(fields);

            break;
        }
        default: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_UNKNOWN, "%s",
                                                in_message.message);
            break;
        }
    }

    device_communication_write_message(window_communication, &out_message);
}

static void queue_message_handler(){
    Message * in_message;
    Queue_message * out_message;
    ConverterResult sender_pid;
    char text[QUEUE_MESSAGE_MESSAGE_LENGTH];
    char **fields;
    int message_id;

    message_id = queue_message_get_message_id(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER);
    in_message = queue_message_receive_message(message_id, QUEUE_MESSAGE_TYPE_DEVICE_START + window->id, true);

    fields = device_communication_split_message_fields(in_message->mesg_text);

    sender_pid = converter_string_to_long(fields[0]);

    snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_WINDOW, QUEUE_MESSAGE_RETURN_NAME_ERROR);

    if(strcmp(fields[1], WINDOW_SWITCH_OPEN) == 0){
        snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_WINDOW, QUEUE_MESSAGE_RETURN_VALUE_ERROR);
        if(strcmp(fields[2], WINDOW_SWITCH_OPEN_OFF) == 0){
            if(window_set_switch_state(WINDOW_SWITCH_OPEN, false)) snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_WINDOW, QUEUE_MESSAGE_RETURN_SUCCESS);
        }
        else if(strcmp(fields[2], WINDOW_SWITCH_OPEN_ON) == 0){
            if(window_set_switch_state(WINDOW_SWITCH_OPEN, true)) snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_WINDOW, QUEUE_MESSAGE_RETURN_SUCCESS);
        }
    }

    out_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER,
                                    QUEUE_MESSAGE_TYPE_DEVICE_START + window->id, text, false);
    queue_message_send_message(out_message);
    queue_message_notify((__pid_t) sender_pid.data.Long);

    device_communication_free_message_fields(fields);
}

int main(int argc, char **args) {
    window = device_child_new_device(argc, args, DEVICE_TYPE_WINDOW, new_window_registry());
    list_add_last(window->switches, new_device_switch(WINDOW_SWITCH_OPEN, (bool *) false,
                                                      (int (*)(const char *, void *)) window_set_switch_state));

    window_communication = device_child_new_device_communication(argc, args, window_message_handler);
    signal(DEVICE_COMMUNICATION_READ_QUEUE, queue_message_handler);

    device_child_run(NULL);

    return EXIT_SUCCESS;
}