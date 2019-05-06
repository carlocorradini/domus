
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

    bulb_switch = device_get_device_switch(bulb->switches, name);
    bulb_registry = (BulbRegistry *) bulb->registry;

    bulb->state = state;
    bulb_switch->state = (bool *) state;
    bulb_registry->start = (state) ? time(NULL) : (time_t) 0;

    return true;
}

static bool bulb_check_value(const char *input) {
    return strcmp(input, "on") == 0 || strcmp(input, "off") == 0;
}

static void bulb_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    device_communication_message_init(bulb, &out_message);

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            time_t on_time = ((BulbRegistry *) bulb->registry)->start;
            double time_difference = (on_time == 0) ? 0.0 : difftime(time(NULL), on_time);
            bool switch_state = (bool) (device_get_device_switch_state(bulb->switches, "turn"));

            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_INFO,
                                                "%d\n%.0lf\n%d\n",
                                                bulb->state, time_difference, switch_state);

            break;
        }
        case MESSAGE_TYPE_SET_INIT_VALUES: {
            fprintf(stderr, "\nINIT VALUES %s\n", in_message.message);

            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SET_INIT_VALUES,
                                                "Ok");
            break;
        }
        case MESSAGE_TYPE_SET_ON: {
            out_message.type = MESSAGE_TYPE_SET_ON;
            char *switch_label;
            char *switch_pos;
            bool bool_switch_pos;

            char **tokenized_result = device_communication_split_message_fields(&in_message);

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
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_ERROR,
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