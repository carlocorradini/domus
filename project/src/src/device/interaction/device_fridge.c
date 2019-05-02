#include <string.h>
#include <time.h>
#include "device/device_child.h"
#include "device/interaction/device_fridge.h"
#include "util/util_converter.h"

/**
 * The Fridge Device
 */
static Device *fridge = NULL;

/**
 * The Device Communication for Fridge
 */
static DeviceCommunication *fridge_communication = NULL;

/**
 * Set the fridge switch state
 * @param name The switch name
 * @param state The state to set
 * @return true if successful, false otherwise
 */
static bool fridge_set_switch_state(const char *name, void * state);

/**
 * Handle the incoming message
 * @param in_message The received message
 */
static void fridge_message_handler(DeviceCommunicationMessage in_message);

/**
 * Check the input value if it is correct
 * @param input The input value param
 * @return true if correct, false otherwise
 */
static bool fridge_check_value(const char *input);

FridgeRegistry *new_fridge_registry(void) {
    FridgeRegistry *fridge_registry;
    if (fridge != NULL) return NULL;

    fridge_registry = (FridgeRegistry *) malloc(sizeof(FridgeRegistry));
    if (fridge_registry == NULL) {
        perror("Fridge Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }

    fridge_registry->time = time(NULL);
    fridge_registry->delay = 0;
    fridge_registry->perc = 0;
    fridge_registry->temp = 4.0;

    return fridge_registry;
}

static bool fridge_set_switch_state(const char *name, void * state) {
    FridgeRegistry *fridge_registry;
    DeviceSwitch *fridge_switch;
    if (!list_contains(fridge->switches, name)) return false;

    if(strcmp(name, "door") == 0){
        if (fridge->state == (bool) state) return true;

        fridge_switch = device_get_device_switch(fridge->switches, name);
        fridge_registry = (FridgeRegistry *) fridge->registry;

        fridge_switch->state = (bool *) state;
        fridge->state = state;
        fridge_registry->time = (state) ? time(NULL) : (time_t) 0;

        return true;
    }
    if(strcmp(name, "thermo") == 0){
        fridge_switch = device_get_device_switch(fridge->switches, name);;

        fridge_registry = (FridgeRegistry *) fridge->registry;
        fridge_switch->state = (double *) state;
        fridge_registry->temp = *((double *) state);

        return true;
    }

    return false;
}

static bool fridge_check_value(const char *input) {
    return strcmp(input, "on") == 0 || strcmp(input, "off") == 0;
}

static void fridge_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    ConverterResult result;

    device_communication_message_init(fridge, &out_message);

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            result = converter_bool_to_string(fridge->state);
            time_t open_time = ((FridgeRegistry *) fridge->registry)->time;
            time_t end_time = time(NULL);
            ConverterResult result_2 = converter_bool_to_string(
                    (bool) (device_get_device_switch_state(fridge->switches, "door")));
            double difference = (open_time == 0) ? 0 : difftime(end_time, open_time);

            double thermostat = *((double * ) (device_get_device_switch_state(fridge->switches, "thermo")));
            device_communication_message_modify(&out_message, MESSAGE_TYPE_INFO,
                                                "ID:%5ld | STATE:%10s | REGISTRY:%8.2f seconds | SWITCH: %10s | THERMOSTAT: %8.2f °C",
                                                fridge->id,
                                                result.data.String,
                                                difference,
                                                result_2.data.String,
                                                thermostat
            );

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

            if(strcmp(switch_label, "door") == 0){
                if (!fridge_check_value(switch_pos)) {
                    device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_ERROR);
                    break;
                }

                bool_switch_pos = strcmp(switch_pos, "on") == 0 ? true : false;

                fridge_set_switch_state(switch_label, (void *) bool_switch_pos)
                ? device_communication_message_modify_message(&out_message, MESSAGE_RETURN_SUCCESS)
                : device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);

                free(tokenized_result);
                break;
            }

            if(strcmp(switch_label, "thermo") == 0){
                ConverterResult result1;
                result1 = converter_string_to_double(switch_pos);

                if(!result1.error){
                    double * temp_result = malloc(sizeof(double));
                    * temp_result = result1.data.Double;
                    fridge_set_switch_state(switch_label, temp_result)
                    ? device_communication_message_modify_message(&out_message, MESSAGE_RETURN_SUCCESS)
                    : device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);

                    free(tokenized_result);
                    break;
                }
            }
        }
        default: {
            device_communication_message_modify(&out_message, MESSAGE_TYPE_ERROR,
                                                "{%d, %s}",
                                                in_message.type,
                                                in_message.message);
            break;
        }
    }

    device_communication_write_message(fridge_communication, &out_message);
}

int main(int argc, char **args) {
    fridge = device_child_new_device(argc, args, new_fridge_registry());
    list_add_last(fridge->switches, new_device_switch("door", DEVICE_STATE, fridge_set_switch_state));

    double * default_tmp = malloc(sizeof(double));
    *default_tmp = DEVICE_FRIDGE_DEFAULT_TEMP;
    list_add_last(fridge->switches, new_device_switch("thermo", (void *) default_tmp, fridge_set_switch_state));

    fridge_communication = device_child_new_device_communication(argc, args, fridge_message_handler);

    device_child_run(NULL);

    return EXIT_SUCCESS;
}