
#include <string.h>
#include <time.h>
#include "device/device_child.h"
#include "device/interaction/device_bulb.h"
#include "util/util_converter.h"

/**
 * The bulb Device
 */
static Device *bulb = NULL;

/**
 * The Device Communication for Bulb
 */
static DeviceCommunication *bulb_communication = NULL;

/**
 * When the bulb lighted up last time
 */
static time_t start = 0;

/**
 * Set the bulb switch state
 * @param name The switch name
 * @param state The state to set
 * @return true if successful, false otherwise
 */
static bool bulb_set_switch_state(const char *name, bool state);

/**
 * Handle the incoming message
 * @param in_message The received message
 */
static void bulb_message_handler(DeviceCommunicationMessage in_message);

/**
 * Check the input value if it is correct
 * @param input The input value param
 * @return true if correct, false otherwise
 */
static bool bulb_check_value(const char *input);

/**
 * The queue_message_handler, it handles the incoming
 * queue messages and send them back
 */
static void queue_message_handler();

BulbRegistry *new_bulb_registry(void) {
    BulbRegistry *bulb_registry;
    if (bulb != NULL) return NULL;

    bulb_registry = (BulbRegistry *) malloc(sizeof(BulbRegistry));
    if (bulb_registry == NULL) {
        perror("Bulb Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }

    bulb_registry->_time = 0;
    start = time(NULL);

    return bulb_registry;
}

static bool bulb_set_switch_state(const char *name, bool state) {
    BulbRegistry *bulb_registry;
    DeviceSwitch *bulb_switch;
    if (!list_contains(bulb->switches, name)) return false;

    if (bulb->state == state) return true;

    if ((bulb_switch = device_get_device_switch(bulb->switches, name)) == NULL) return false;
    bulb_registry = (BulbRegistry *) bulb->registry;

    bulb->state = state;
    bulb_switch->state = (bool *) state;
    if (state && start == 0) {
        start = time(NULL);
    } else {
        bulb_registry->_time = difftime(time(NULL), start);
        start = 0;
    }

    return true;
}

static bool bulb_check_value(const char *input) {
    return strcmp(input, BULB_SWITCH_TURN_ON) == 0 || strcmp(input, BULB_SWITCH_TURN_OFF) == 0;
}

static void bulb_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    device_communication_message_init(bulb, &out_message);
    out_message.override = bulb->override;

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            double on_time = ((BulbRegistry *) bulb->registry)->_time;
            bool switch_state = (bool) (device_get_device_switch_state(bulb->switches, BULB_SWITCH_TURN));
            double time_difference = (switch_state) ? difftime(time(NULL), start) : on_time;
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_INFO,
                                                "%d\n%.0lf\n%d\n",
                                                bulb->state, time_difference, switch_state);
            break;
        }
        case MESSAGE_TYPE_SET_INIT_VALUES: {
            ConverterResult state;
            ConverterResult start_time;
            ConverterResult switch_state;
            char **fields;

            fields = device_communication_split_message_fields(in_message.message);

            state = converter_char_to_bool(fields[2][0]);
            start_time = converter_string_to_long(fields[3]);
            switch_state = converter_char_to_bool(fields[4][0]);

            bulb->state = state.data.Bool;

            device_get_device_switch(bulb->switches, BULB_SWITCH_TURN)->state = (bool *) switch_state.data.Bool;

            if ((bool) device_get_device_switch(bulb->switches, BULB_SWITCH_TURN)->state) {
                start = time(NULL) - start_time.data.Long;
            } else {
                ((BulbRegistry *) bulb->registry)->_time = start_time.data.Long;
                start = 0;
            }

            device_communication_free_message_fields(fields);
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SET_INIT_VALUES,
                                                "");
            break;
        }
        case MESSAGE_TYPE_SWITCH: {
            char **fields;

            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SWITCH, "");
            fields = device_communication_split_message_fields(in_message.message);

            bulb->override = in_message.override;

            if (device_get_device_switch(bulb->switches, fields[0]) == NULL) {
                device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);
            } else if (!bulb_check_value(fields[1])) {
                device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_ERROR);
            } else {
                bulb_set_switch_state(fields[0], strcmp(fields[1], BULB_SWITCH_TURN_ON) == 0)
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

    device_communication_write_message(bulb_communication, &out_message);
}

static void queue_message_handler() {
    Message *in_message;
    Queue_message *out_message;
    ConverterResult sender_pid;
    char text[QUEUE_MESSAGE_MESSAGE_LENGTH];
    char **fields;
    int message_id;

    message_id = queue_message_get_message_id(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER);
    in_message = queue_message_receive_message(message_id, QUEUE_MESSAGE_TYPE_DEVICE_START + bulb->id, true);

    fields = device_communication_split_message_fields(in_message->mesg_text);

    sender_pid = converter_string_to_long(fields[0]);

    snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_BULB, QUEUE_MESSAGE_RETURN_NAME_ERROR);

    if (strcmp(fields[1], BULB_SWITCH_TURN) == 0) {
        snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_BULB, QUEUE_MESSAGE_RETURN_VALUE_ERROR);
        if (strcmp(fields[2], BULB_SWITCH_TURN_OFF) == 0) {
            if (bulb_set_switch_state(BULB_SWITCH_TURN, false)) {
                snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_BULB, QUEUE_MESSAGE_RETURN_SUCCESS);
                bulb->override = true;
            }
        } else if (strcmp(fields[2], BULB_SWITCH_TURN_ON) == 0) {
            if (bulb_set_switch_state(BULB_SWITCH_TURN, true)) {
                snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_BULB, QUEUE_MESSAGE_RETURN_SUCCESS);
                bulb->override = true;
            }
        }
    }

    out_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER,
                                    QUEUE_MESSAGE_TYPE_DEVICE_START + bulb->id, text, false);
    queue_message_send_message(out_message);
    queue_message_notify((__pid_t) sender_pid.data.Long);

    device_communication_free_message_fields(fields);
}

int main(int argc, char **args) {
    bulb = device_child_new_device(argc, args, DEVICE_TYPE_BULB, new_bulb_registry());
    bulb->override = false;
    list_add_last(bulb->switches, new_device_switch(BULB_SWITCH_TURN, (bool *) DEVICE_STATE,
                                                    (int (*)(const char *, void *)) bulb_set_switch_state));
    bulb_communication = device_child_new_device_communication(argc, args, bulb_message_handler);

    signal(DEVICE_COMMUNICATION_READ_QUEUE, queue_message_handler);
    device_child_run(NULL);

    return EXIT_SUCCESS;
}