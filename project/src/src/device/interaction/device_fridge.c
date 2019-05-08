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
 * Internal timer that handles the signals when the door is left open
 * for more then delay_time
 */
static timer_t door_timer;

/**
 * The event thrown by the timer
 * @return
 */
static struct sigevent sigevent;

/**
 * Aux structure for timer_settime
 */
static struct itimerspec t;


/**
 * Set the fridge switch state
 * @param name The switch name
 * @param state The state to set
 * @return true if successful, false otherwise
 */
static bool fridge_set_switch_state(const char *name, void *state);

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
    fridge_registry->delay = DEVICE_FRIDGE_DEFAULT_DELAY;
    fridge_registry->perc = DEVICE_FRIDGE_DEFAULT_PERC;
    fridge_registry->temp = DEVICE_FRIDGE_DEFAULT_TEMP;

    return fridge_registry;
}

static bool fridge_set_switch_state(const char *name, void *state) {
    FridgeRegistry *fridge_registry;
    DeviceSwitch *fridge_switch;
    if (!list_contains(fridge->switches, name)) return false;

    if (strcmp(name, "door") == 0) {

        fridge_switch = device_get_device_switch(fridge->switches, name);
        fridge_registry = (FridgeRegistry *) fridge->registry;

        fridge_switch->state = (bool *) state;
        fridge_registry->time = (state) ? time(NULL) : (time_t) 0;

        if ((bool) fridge_switch->state == true) {
            if (door_timer == NULL) {

                sigevent.sigev_notify = SIGEV_SIGNAL;
                sigevent.sigev_signo = DEVICE_COMMUNICATION_TIMER;
                sigevent.sigev_value.sival_ptr = &door_timer;

                if (timer_create(CLOCK_REALTIME, &sigevent, &door_timer) != 0) {
                    fprintf(stderr, "\nError while initializing the door timer\n");
                    return false;
                }
                t.it_value.tv_sec = fridge_registry->delay;
                timer_settime(door_timer, 0, &t, NULL);
            }
        }
        return true;
    }
    if (strcmp(name, "thermo") == 0) {
        fridge_switch = device_get_device_switch(fridge->switches, name);

        fridge_registry = (FridgeRegistry *) fridge->registry;
        fridge_switch->state = (double *) state;
        fridge_registry->temp = *((double *) state);

        return true;
    }
    if (strcmp(name, "state") == 0) {
        fridge_switch = device_get_device_switch(fridge->switches, name);

        fridge_switch->state = (bool *) state;
        fridge->state = state;
        return true;
    }
    if (strcmp(name, "delay") == 0) {
        fridge_switch = device_get_device_switch(fridge->switches, name);

        fridge_registry = (FridgeRegistry *) fridge->registry;
        fridge_switch->state = (long *) state;
        fridge_registry->delay = *((long *) state);


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
            const FridgeRegistry *fridge_registry = (FridgeRegistry *) fridge->registry;
            double time_difference = (fridge_registry->time == 0) ? 0.0 : difftime(time(NULL), fridge_registry->time);
            long delay = fridge_registry->delay;
            float perc = fridge_registry->perc;
            double temp = fridge_registry->temp;
            bool switch_door = (bool) (device_get_device_switch_state(fridge->switches, "door"));
            if(switch_door == false){
                time_difference = 0;
            }
            double switch_thermo = *((double *) (device_get_device_switch_state(fridge->switches, "thermo")));

            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_INFO,
                                                "%d\n%.0lf\n%ld\n%.2f\n%.2lf\n%.0lf\n%d\n",
                                                fridge->state, time_difference, delay, perc, temp, switch_thermo,
                                                switch_door);

            break;
        }
        case MESSAGE_TYPE_SET_INIT_VALUES: {

            ConverterResult state;
            ConverterResult open_time;
            ConverterResult delay_time;
            ConverterResult perc;
            ConverterResult temp;
            ConverterResult switch_thermo;
            ConverterResult switch_door;

            char **fields = device_communication_split_message_fields(&in_message);

            state = converter_char_to_bool(fields[2][0]);
            open_time = converter_string_to_long(fields[3]);
            delay_time = converter_string_to_long(fields[4]);
            perc = converter_string_to_double(fields[5]);
            temp = converter_string_to_double(fields[6]);
            switch_thermo = converter_char_to_bool(fields[7][0]);
            switch_door = converter_char_to_bool(fields[8][0]);


            fridge->state = state.data.Bool;
            ((FridgeRegistry *) fridge->registry)->time = time(NULL) - open_time.data.Long;
            ((FridgeRegistry *) fridge->registry)->delay = delay_time.data.Long;
            ((FridgeRegistry *) fridge->registry)->perc = perc.data.Double;
            ((FridgeRegistry *) fridge->registry)->temp = temp.data.Double;

            double *thermo_tmp = malloc(sizeof(double));
            *thermo_tmp = switch_thermo.data.Double;
            device_get_device_switch(fridge->switches, "thermo")->state = thermo_tmp;
            device_get_device_switch(fridge->switches, "door")->state = (bool *) switch_door.data.Bool;

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

            char **fields = device_communication_split_message_fields(&in_message);

            switch_label = fields[0];
            switch_pos = fields[1];

            if (strcmp(switch_label, "door") == 0) {
                if (!fridge_check_value(switch_pos)) {
                    device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_ERROR);
                    break;
                }

                bool_switch_pos = strcmp(switch_pos, "on") == 0 ? true : false;

                fridge_set_switch_state(switch_label, (void *) bool_switch_pos)
                ? device_communication_message_modify_message(&out_message, MESSAGE_RETURN_SUCCESS)
                : device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);

                device_communication_free_message_fields(fields);
                break;
            }

            if (strcmp(switch_label, "thermo") == 0) {
                ConverterResult result1;
                result1 = converter_string_to_double(switch_pos);

                if (!result1.error) {
                    double *temp_result = malloc(sizeof(double));
                    *temp_result = result1.data.Double;
                    fridge_set_switch_state(switch_label, temp_result)
                    ? device_communication_message_modify_message(&out_message, MESSAGE_RETURN_SUCCESS)
                    : device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);

                    device_communication_free_message_fields(fields);
                    break;
                }
            }

            if (strcmp(switch_label, "state") == 0) {
                bool_switch_pos = strcmp(switch_pos, "on") == 0 ? true : false;

                fridge_set_switch_state(switch_label, (void *) bool_switch_pos)
                ? device_communication_message_modify_message(&out_message, MESSAGE_RETURN_SUCCESS)
                : device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);

                device_communication_free_message_fields(fields);
                break;
            }
            if (strcmp(switch_label, "delay") == 0) {
                ConverterResult result1;
                result1 = converter_string_to_long(switch_pos);

                if (!result1.error) {
                    long *delay_result = malloc(sizeof(long));
                    *delay_result = result1.data.Long;
                    fridge_set_switch_state(switch_label, delay_result)
                    ? device_communication_message_modify_message(&out_message, MESSAGE_RETURN_SUCCESS)
                    : device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);

                    device_communication_free_message_fields(fields);
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

    device_communication_write_message(fridge_communication,
                                       &out_message);
}


static void close_door() {
    fridge_set_switch_state("door", (void *) false);
    perror("Here close\n");
    door_timer = NULL;
}


int main(int argc, char **args) {
    fridge = device_child_new_device(argc, args, new_fridge_registry());
    list_add_last(fridge->switches, new_device_switch("state", DEVICE_STATE, fridge_set_switch_state));
    list_add_last(fridge->switches, new_device_switch("door", DEVICE_FRIDGE_DEFAULT_DOOR, fridge_set_switch_state));

    double *default_tmp = malloc(sizeof(double));
    *default_tmp = DEVICE_FRIDGE_DEFAULT_TEMP;
    list_add_last(fridge->switches, new_device_switch("thermo", (void *) default_tmp, fridge_set_switch_state));
    long *default_delay = malloc(sizeof(long));
    *default_delay = DEVICE_FRIDGE_DEFAULT_DELAY;
    list_add_last(fridge->switches, new_device_switch("delay", (void *) default_delay, fridge_set_switch_state));

    fridge_communication = device_child_new_device_communication(argc, args, fridge_message_handler);

    signal(DEVICE_COMMUNICATION_TIMER, close_door);
    device_child_run(NULL);

    return EXIT_SUCCESS;
}