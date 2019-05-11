
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

            char **fields = device_communication_split_message_fields(&in_message);

            state = converter_char_to_bool(fields[2][0]);
            start_time = converter_string_to_long(fields[3]);
            switch_state = converter_char_to_bool(fields[4][0]);

            bulb->state = state.data.Bool;

            device_get_device_switch(bulb->switches, BULB_SWITCH_TURN)->state = (bool *) switch_state.data.Bool;

            if ((bool) device_get_device_switch(bulb->switches, BULB_SWITCH_TURN)->state) {
                start = time(NULL) - start_time.data.Long;
            } else {
                ((BulbRegistry *) bulb->registry)->_time = start_time.data.Long;
            }

            device_communication_free_message_fields(fields);
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SET_INIT_VALUES,
                                                "");
            break;
        }
        case MESSAGE_TYPE_SET_ON: {
            const char *switch_label;
            const char *switch_pos;

            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SET_ON, "");
            char **fields = device_communication_split_message_fields(&in_message);

            switch_label = fields[0];
            switch_pos = fields[1];

            if (device_get_device_switch(bulb->switches, switch_label) == NULL) {
                device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);
            } else if (!bulb_check_value(switch_pos)) {
                device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_ERROR);
            } else {
                bulb_set_switch_state(switch_label, strcmp(switch_pos, BULB_SWITCH_TURN_ON) == 0)
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

int main(int argc, char **args) {
    bulb = device_child_new_device(argc, args, DEVICE_TYPE_BULB, new_bulb_registry());
    list_add_last(bulb->switches, new_device_switch(BULB_SWITCH_TURN, (bool *) DEVICE_STATE, bulb_set_switch_state));
    bulb_communication = device_child_new_device_communication(argc, args, bulb_message_handler);

    device_child_run(NULL);

    return EXIT_SUCCESS;
}