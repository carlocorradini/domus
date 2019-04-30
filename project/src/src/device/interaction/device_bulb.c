
#include <string.h>
#include <time.h>
#include "device/device_child.h"
#include "device/interaction/device_bulb.h"
#include "util/util_converter.h"
#include "util/util_string_handler.h"

/**
 * The bulb Device
 */
static Device *bulb = NULL;

/**
 * The Device Communication for Bulb
 */
static DeviceCommunication *bulb_communication = NULL;

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

    bulb_registry->start = time(NULL);

    return bulb_registry;
}

static bool bulb_set_switch_state(const char *name, bool state) {
    BulbRegistry *bulb_registry;
    DeviceSwitch *bulb_switch;
    if (!list_contains(bulb->switches, name)) return false;

    if (bulb->state == state) return true;

    bulb_switch = get_device_switch(bulb->switches, name);
    bulb_registry = (BulbRegistry *) bulb->registry;

    bulb_switch->state = (bool *) state;
    bulb->state = state;
    bulb_registry->start = (state) ? time(NULL) : (time_t) 0;

    return true;
}

static bool bulb_check_value(const char *input) {
    return strcmp(input, "on") == 0 || strcmp(input, "off") == 0;
}

static void bulb_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    ConverterResult result;

    device_communication_message_init(bulb, &out_message);

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            result = converter_bool_to_string(bulb->state);
            time_t open_time = ((BulbRegistry *) bulb->registry)->start;
            time_t end_time = time(NULL);
            ConverterResult result_2 = converter_bool_to_string(
                    (bool ) (get_device_switch_state(bulb->switches, "turn")));
            double difference = (open_time == 0) ? 0 : difftime(end_time, open_time);

            device_communication_message_modify(&out_message, MESSAGE_TYPE_INFO,
                                                "ID:%5ld | STATE:%10s | REGISTRY:%8.2f seconds | SWITCH: %10s",
                                                bulb->id,
                                                result.data.String,
                                                difference,
                                                result_2.data.String
            );

            break;
        }
        case MESSAGE_TYPE_SET_ON: {
            out_message.type = MESSAGE_TYPE_SET_ON;
            char *switch_label;
            char *switch_pos;
            bool bool_switch_pos;

            char **tokenized_result = string_to_string_array(in_message.message);

            switch_label = tokenized_result[0];
            switch_pos = tokenized_result[1];

            if (!bulb_check_value(switch_pos)) {
                device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_ERROR);
                break;
            }

            bool_switch_pos = strcmp(switch_pos, "on") == 0 ? true : false;

            bulb_set_switch_state(switch_label, bool_switch_pos)
            ? device_communication_message_modify_message(&out_message, MESSAGE_RETURN_SUCCESS)
            : device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);

            free(tokenized_result);
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

    device_communication_write_message(bulb_communication, &out_message);
}

int main(int argc, char **args) {
    bulb = device_child_new_device(argc, args, new_bulb_registry());
    list_add_last(bulb->switches, new_device_switch("turn", DEVICE_STATE, bulb_set_switch_state));

    bulb_communication = device_child_new_device_communication(argc, args, bulb_message_handler);

    device_child_run(NULL);

    return EXIT_SUCCESS;
}