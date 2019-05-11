
#include <time.h>
#include <string.h>
#include "device/control/device_timer.h"
#include "device/device_child.h"
#include "util/util_converter.h"
#include "device/device_communication.h"

/**
 *  The Timer Control Device
 */
static ControlDevice *timer = NULL;

/**
 * The Device Communication for Timer
 */
static DeviceCommunication *timer_communication = NULL;

/**
 * Handle the incoming message
 * @param in_message The received message
 */
static void timer_message_handler(DeviceCommunicationMessage in_message);

/**
 * Internal timer that handles the signals
 */
static timer_t internal_timer;

/**
 * The event thrown by the timer
 */
static struct sigevent sigevent;

/**
 * Aux structure for timer_settime
 */
static struct itimerspec t;

TimerRegistry *new_timer_registry(void) {
    TimerRegistry *timer_registry;
    if (timer != NULL) return NULL;

    timer_registry = (TimerRegistry *) malloc(sizeof(TimerRegistry));
    if (timer_registry == NULL) {
        perror("Timer Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }
    timer_registry->begin = 0;
    timer_registry->end = 0;

    return timer_registry;
}

static bool timer_set_switch_state(const char *name, char *dates) {
    TimerRegistry *timer_registry;
    const char *start_date;
    const char *end_date;
    ConverterResult result;

    if (!list_contains(timer->device->switches, name)) return false;

    timer_registry = (TimerRegistry *) timer->device->registry;
    start_date = strtok(dates, TIMER_DATE_DELIMITER);
    end_date = strtok(NULL, TIMER_DATE_DELIMITER);
    result = converter_string_to_date(start_date);

    if (result.error) {
        fprintf(stderr, "%s\n", result.error_message);
        return false;
    }

    timer_registry->begin = mktime(&result.data.Date);
    if (timer_registry->begin == -1) {
        fprintf(stderr, "Please enter a valid date\n");
        return false;
    }

    result = converter_string_to_date(end_date);
    if (result.error) {
        fprintf(stderr, "%s\n", result.error_message);
        return false;
    }

    timer_registry->end = mktime(&result.data.Date);
    if (timer_registry->end == -1) {
        fprintf(stderr, "Please enter a valid date\n");
        return false;
    }

    if (difftime(timer_registry->end, timer_registry->begin) < 0) {
        fprintf(stderr, "%s\n", result.error_message);
        return false;
    }

    if (internal_timer == NULL) {
        sigevent.sigev_notify = SIGEV_SIGNAL;
        sigevent.sigev_signo = DEVICE_COMMUNICATION_TIMER;
        sigevent.sigev_value.sival_ptr = &internal_timer;

        if (timer_create(CLOCK_REALTIME, &sigevent, &internal_timer) != 0) {
            fprintf(stderr, "Error while initializing the internal timer\n");
            return false;
        }
        t.it_value.tv_sec = timer_registry->begin - time(NULL);
        timer_settime(internal_timer, 0, &t, NULL);
    } else {
        fprintf(stderr, "Please wait until the timer expires\n");
        return false;
    }
    return true;
}

static void timer_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    device_communication_message_init(timer->device, &out_message);

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            time_t start_time = ((TimerRegistry *) timer->device->registry)->begin;
            time_t end_time = ((TimerRegistry *) timer->device->registry)->end;

            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_INFO,
                                                "%d\n%ld\n%ld\n", timer->device->state, start_time, end_time);
            break;
        }
        case MESSAGE_TYPE_SET_INIT_VALUES: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SET_INIT_VALUES,
                                                "");
            break;
        }
        case MESSAGE_TYPE_SET_ON: {
            char **fields;

            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SET_ON, "");
            fields = device_communication_split_message_fields(&in_message);

            if (device_get_device_switch(timer->device->switches, fields[0]) == NULL) {
                device_communication_message_modify_message(&out_message, MESSAGE_RETURN_NAME_ERROR);
            } else {
                timer_set_switch_state(fields[0], fields[1])
                ? device_communication_message_modify_message(&out_message, MESSAGE_RETURN_SUCCESS)
                : device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_ERROR);
            }

            device_communication_free_message_fields(fields);

            break;
        }

        case MESSAGE_TYPE_SPAWN_DEVICE: {
            if (!list_is_empty(timer->devices)) {
                device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_ERROR,
                                                    "Cannot attach more than one device per timer");
            } else {
                device_child_set_device_to_spawn(in_message);
                return;
            }

            break;
        }

        default: {
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_UNKNOWN, "%s",
                                                in_message.message);
            break;
        }
    }

    device_communication_write_message(timer_communication, &out_message);
}

static void set_device() {
    /**
     * First of all, get informations about the device id and descriptor in
     * order to know what switches to set
     */
    char switch_name[DEVICE_SWITCH_NAME_LENGTH];
    size_t device_id;
    size_t device_descriptor;
    DeviceCommunicationMessage send_message;
    device_communication_message_init(timer->device, &send_message);

    device_communication_message_modify(&send_message, timer->device->id, MESSAGE_TYPE_INFO, "");

    send_message = device_communication_write_message_with_ack((DeviceCommunication *) list_get_first(timer->devices),
                                                               &send_message);

    device_id = send_message.id_sender;
    device_descriptor = send_message.id_device_descriptor;

    switch (device_descriptor) {
        case DEVICE_TYPE_BULB : {
            strcpy(switch_name, "turn");
            break;
        }
        case DEVICE_TYPE_WINDOW : {
            strcpy(switch_name, "open");
            break;
        }
        case DEVICE_TYPE_FRIDGE : {
            strcpy(switch_name, "state");
            break;
        }
        default: {
            fprintf(stderr, "Device not supported yet\n");
            exit(EXIT_FAILURE);
        }
    }

    /**
     * Set the switches
     */
    t.it_value.tv_sec = ((TimerRegistry *) timer->device->registry)->end - time(NULL);
    if (t.it_value.tv_sec > 0) {
        device_communication_message_modify(&send_message, device_id, MESSAGE_TYPE_SET_ON, "%s\noff\n", switch_name);
        timer_settime(internal_timer, 0, &t, NULL);
    } else {
        device_communication_message_modify(&send_message, device_id, MESSAGE_TYPE_SET_ON, "%s\non\n", switch_name);
        timer_settime(internal_timer, 0, &t, NULL);
        internal_timer = NULL;
    }
    device_communication_write_message_with_ack((DeviceCommunication *) list_get_first(timer->devices), &send_message);


}

int main(int argc, char **args) {
    timer = device_child_new_control_device(argc, args, DEVICE_TYPE_TIMER, new_timer_registry());
    list_add_last(timer->device->switches,
                  new_device_switch(TIMER_SWITCH_TIME, (bool *) DEVICE_STATE, timer_set_switch_state));
    timer_communication = device_child_new_control_device_communication(argc, args, timer_message_handler);

    signal(DEVICE_COMMUNICATION_TIMER, set_device);

    device_child_run(NULL);

    return EXIT_SUCCESS;
}