
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
 * @return
 */
static struct sigevent sigevent;

/**
 * Aux structure for timer_settime
 */
static struct itimerspec t;

/**
 * The queue_message_handler, it handles the incoming
 * queue messages and send them back
 */
static void queue_message_handler();

TimerRegistry *new_timer_registry(void) {
    TimerRegistry *timer_registry;
    if (timer != NULL) return NULL;

    timer_registry = (TimerRegistry *) malloc(sizeof(TimerRegistry));
    if (timer_registry == NULL) {
        perror("Hub Registry Memory Allocation");
        exit(EXIT_FAILURE);
    }
    timer_registry->begin.tm_year = 0;
    timer_registry->end.tm_year = 0;

    return timer_registry;
}

static int timer_set_switch_state(const char *name, char *dates) {
    TimerRegistry *timer_registry;

    if (!list_contains(timer->device->switches, name)) return -1;

    char *start_date = strtok(dates, TIMER_DATE_DELIMITER);
    char *end_date = strtok(NULL, TIMER_DATE_DELIMITER);

    timer_registry = (TimerRegistry *) timer->device->registry;

    ConverterResult start = converter_string_to_date(start_date);

    if (start.error) {
        if (strcmp(start.error_message, "Format") == 0) {
            return -1;
        }
        if (strcmp(start.error_message, "Passed") == 0) {
            return -2;
        }
        return 0;
    }
    timer_registry->begin = start.data.Date;

    ConverterResult end = converter_string_to_date(end_date);

    if (end.error) {
        if (strcmp(end.error_message, "Format") == 0) {
            return -1;
        }
        if (strcmp(end.error_message, "Passed") == 0) {
            return -2;
        }
        return 0;
    }

    timer_registry->end = end.data.Date;

    if (difftime(mktime(&timer_registry->end), mktime(&timer_registry->begin)) < 0) {
        return -3;
    }
    if (difftime(mktime(&timer_registry->end), mktime(&timer_registry->begin)) == 0) {
        return -6;
    }

    if (internal_timer == NULL) {


        sigevent.sigev_notify = SIGEV_SIGNAL;
        sigevent.sigev_signo = DEVICE_COMMUNICATION_TIMER;
        sigevent.sigev_value.sival_ptr = &internal_timer;

        if (timer_create(CLOCK_REALTIME, &sigevent, &internal_timer) != 0) {
            fprintf(stderr, "\tError while initializing the internal timer\n");
            return 0;
        }
        t.it_value.tv_sec = mktime(&timer_registry->begin) - time(NULL);
        timer_settime(internal_timer, 0, &t, NULL);
    } else {
        return -5;
    }
    return 1;
}

static void timer_message_handler(DeviceCommunicationMessage in_message) {
    DeviceCommunicationMessage out_message;
    ConverterResult result;

    device_communication_message_init(timer->device, &out_message);

    switch (in_message.type) {
        case MESSAGE_TYPE_INFO: {
            ConverterResult start_date, end_date;

            start_date = converter_date_to_string(&(((TimerRegistry *) timer->device->registry)->begin));
            end_date = converter_date_to_string(&(((TimerRegistry *) timer->device->registry)->end));

            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_INFO,
                                                "%d\n%s\n%s", timer->device->state,
                                                (((TimerRegistry *) timer->device->registry)->begin.tm_year == 0) ? "NOT SET" : start_date.data.String,
                                                (((TimerRegistry *) timer->device->registry)->end.tm_year == 0)  ? "NOT SET" : end_date.data.String);

            break;
        }
        case MESSAGE_TYPE_SET_INIT_VALUES: {
            char **fields = device_communication_split_message_fields(in_message.message);

            result = converter_char_to_bool(fields[2][0]);

            timer->device->state = result.data.Bool;

            if(strcmp(fields[3], "NOT SET") == 0){
                ((TimerRegistry *) timer->device->registry)->begin.tm_year = 0;
                ((TimerRegistry *) timer->device->registry)->end.tm_year = 0;
            }
            else{
                ConverterResult date1, date2;
                date1 = converter_string_to_date(fields[3]);
                ((TimerRegistry *) timer->device->registry)->begin = date1.data.Date;
                date2 = converter_string_to_date(fields[4]);
                ((TimerRegistry *) timer->device->registry)->end = date2.data.Date;
            }

            device_communication_free_message_fields(fields);
            device_communication_message_modify(&out_message, in_message.id_sender, MESSAGE_TYPE_SET_INIT_VALUES,
                                                "");
            break;
        }
        case MESSAGE_TYPE_SWITCH: {
            out_message.type = MESSAGE_TYPE_SWITCH;
            int res;

            char **fields = device_communication_split_message_fields(in_message.message);

            res = (timer_set_switch_state(fields[0], fields[1]));
            switch (res) {
                case 1 : {
                    device_communication_message_modify_message(&out_message, MESSAGE_RETURN_SUCCESS);
                    break;
                }
                case -1 : {
                    device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_FORMAT_DATE_ERROR);
                    break;
                }
                case -2 : {
                    device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_PASSED_DATE_ERROR);
                    break;
                }
                case -3 : {
                    device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_ORDER_DATE_ERROR);
                    break;
                }
                case -4 : {
                    device_communication_message_modify_message(&out_message, QUEUE_MESSAGE_RETURN_NAME_ERROR);
                    break;
                }
                case -5 : {
                    device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_ALREADY_DEFINED_DATE_ERROR);
                    break;
                }
                case -6 : {
                    device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_SAME_DATE_ERROR);
                    break;
                }
                default: {
                    device_communication_message_modify_message(&out_message, MESSAGE_RETURN_VALUE_ERROR);
                }
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
    if (list_get_first(timer->devices) != NULL) {
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

        send_message = device_communication_write_message_with_ack(
                (DeviceCommunication *) list_get_first(timer->devices),
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
                fprintf(stderr, "\tDevice not supported yet\n");
                exit(EXIT_FAILURE);
            }
        }

        /**
         * Set the switches
         */
        t.it_value.tv_sec = mktime(&((TimerRegistry *) timer->device->registry)->end) - time(NULL);
        if (t.it_value.tv_sec > 0) {
            device_communication_message_modify(&send_message, device_id, MESSAGE_TYPE_SWITCH, "%s\noff\n",
                                                switch_name);
            timer_settime(internal_timer, 0, &t, NULL);
        } else {
            device_communication_message_modify(&send_message, device_id, MESSAGE_TYPE_SWITCH, "%s\non\n", switch_name);
            timer_settime(internal_timer, 0, &t, NULL);
            internal_timer = NULL;
            ((TimerRegistry *) timer->device->registry)->begin.tm_year = 0;
            ((TimerRegistry *) timer->device->registry)->end.tm_year = 0;
        }
        device_communication_write_message_with_ack((DeviceCommunication *) list_get_first(timer->devices),
                                                    &send_message);

    }
}

static void queue_message_handler() {
    Message *in_message;
    Queue_message *out_message;
    DeviceCommunicationMessage *fake_message;
    ConverterResult sender_pid;
    char text[QUEUE_MESSAGE_MESSAGE_LENGTH];
    char **fields;
    int message_id;

    message_id = queue_message_get_message_id(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER);
    in_message = queue_message_receive_message(message_id, QUEUE_MESSAGE_TYPE_DEVICE_START + timer->device->id, true);

    fake_message = malloc(sizeof(DeviceCommunicationMessage));
    device_communication_message_modify_message(fake_message, in_message->mesg_text);

    fields = device_communication_split_message_fields(fake_message->message);

    sender_pid = converter_string_to_long(fields[0]);

    snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_TIMER, QUEUE_MESSAGE_RETURN_NAME_ERROR);

    if (strcmp(fields[1], TIMER_SWITCH_TIME) == 0) {
        snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_TIMER, QUEUE_MESSAGE_RETURN_VALUE_ERROR);

        int res;

        res = (timer_set_switch_state(fields[1], fields[2]));
        switch (res) {
            case 1 : {
                snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_TIMER, MESSAGE_RETURN_SUCCESS);
                break;
            }
            case -1 : {
                snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_TIMER, MESSAGE_RETURN_VALUE_FORMAT_DATE_ERROR);
                break;
            }
            case -2 : {
                snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_TIMER, MESSAGE_RETURN_VALUE_PASSED_DATE_ERROR);
                break;
            }
            case -3 : {
                snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_TIMER, MESSAGE_RETURN_VALUE_ORDER_DATE_ERROR);
                break;
            }
            case -4 : {
                snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_TIMER, QUEUE_MESSAGE_RETURN_NAME_ERROR);
                break;
            }
            case -5 : {
                snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_TIMER, MESSAGE_RETURN_VALUE_ALREADY_DEFINED_DATE_ERROR);
                break;
            }
            case -6 : {
                snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_TIMER, MESSAGE_RETURN_VALUE_SAME_DATE_ERROR);
                break;
            }
            default: {
                snprintf(text, 64, "%d\n%s\n", DEVICE_TYPE_TIMER, MESSAGE_RETURN_VALUE_ERROR);
            }
        }


    }

    out_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER,
                                    QUEUE_MESSAGE_TYPE_DEVICE_START + timer->device->id, text, false);
    queue_message_send_message(out_message);
    queue_message_notify((__pid_t) sender_pid.data.Long);

    free(fake_message);
    device_communication_free_message_fields(fields);
}

int main(int argc, char **args) {
    timer = device_child_new_control_device(argc, args, DEVICE_TYPE_TIMER, new_timer_registry());
    list_add_last(timer->device->switches, new_device_switch(TIMER_SWITCH_TIME, (bool *) DEVICE_STATE,
                                                             (int (*)(const char *, void *)) timer_set_switch_state));
    timer_communication = device_child_new_control_device_communication(argc, args, timer_message_handler);

    signal(DEVICE_COMMUNICATION_TIMER, set_device);
    signal(DEVICE_COMMUNICATION_READ_QUEUE, queue_message_handler);
    device_child_run(NULL);

    return EXIT_SUCCESS;
}