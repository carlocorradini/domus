
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
            char **fields;

            fields = device_communication_split_message_fields(&in_message);

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
            char **fields;

            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SET_ON, "");
            fields = device_communication_split_message_fields(&in_message);

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
#include <sys/ipc.h>
#include <sys/msg.h>

typedef struct _message {
    long mesg_type;
    char mesg_text[100];
} _message;

static key_t child_message_key;
static int child_message_id;
static _message child_in_message;

static void get_child_queue_message();

static char *queue_message = NULL;

void get_child_queue_message() {

    /**
     * Check wheter the device is a control one or not
     */
    /*
    if (control_device_child != NULL && device_child == NULL) {
        child_message_key = ftok("control", control_device_child->device->id);
    } else if (device_child != NULL && control_device_child == NULL) {
        child_message_key = ftok("control", device_child->id);
    }
    */
    child_message_key = ftok("control", 4);
    child_message_id = msgget(child_message_key, 0666 | IPC_CREAT);

    if (msgrcv(child_message_id, &child_in_message, sizeof(child_in_message), 1, IPC_NOWAIT) != -1) {
        fprintf(stderr, "\t%s\n", child_in_message.mesg_text);
    }
    msgctl(child_message_id, IPC_RMID, NULL);
}

int main(int argc, char **args) {
    bulb = device_child_new_device(argc, args, DEVICE_TYPE_BULB, new_bulb_registry());
    list_add_last(bulb->switches, new_device_switch(BULB_SWITCH_TURN, (bool *) DEVICE_STATE, bulb_set_switch_state));
    bulb_communication = device_child_new_device_communication(argc, args, bulb_message_handler);
    signal(SIGUSR2, get_child_queue_message);
    device_child_run(NULL);

    return EXIT_SUCCESS;
}