#include "device/manual_libs.h"
#include "util/util_printer.h"

/**
 * The domus_pid
 *  If 0, not connected yet
 */
static pid_t domus_pid = 0;

bool manual_control_check_domus(pid_t pid) {
    ConverterResult in_pid;
    Queue_message *check_message;
    Message *in_message;
    char text[QUEUE_MESSAGE_MESSAGE_LENGTH];

    snprintf(text, 64, "%d", getpid());
    check_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER,
                                      QUEUE_MESSAGE_TYPE_DOMUS_PID_REQUEST, text, true);

    queue_message_send_message(check_message);
    queue_message_notify(pid);
    usleep(DOMUS_GET_PID_SLEEP);

    in_message = queue_message_receive_message(check_message->message_id, QUEUE_MESSAGE_TYPE_DOMUS_PID_REQUEST, false);
    if (in_message != NULL) {
        in_pid = converter_string_to_long(in_message->mesg_text);

        if (in_pid.error || in_pid.data.Long != pid) {
            return false;
        }

        domus_pid = pid;
        return true;
    }
    return false;
}

pid_t manual_control_get_device_pid(size_t device_id) {
    if (domus_pid == 0) {
        println_color(COLOR_RED, "\tPlease connect to domus first");
        return -1;
    }
    ConverterResult out_pid;
    Queue_message *check_message;
    Message *in_message;
    char text[QUEUE_MESSAGE_MESSAGE_LENGTH];

    snprintf(text, 64, "%lu\n%d\n", device_id, getpid());
    check_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER,
                                      QUEUE_MESSAGE_TYPE_PID_REQUEST, text, true);

    in_message = queue_message_send_message_with_ack(domus_pid, check_message);

    out_pid = converter_string_to_long(in_message->mesg_text);

    if (out_pid.error) {
        return 0;
    }
    return out_pid.data.Long;
}

void manual_control_set_device(size_t device_id, char *switch_label, char *switch_pos) {
    pid_t device_pid;
    Queue_message *out_message;
    Message *in_message;
    ConverterResult descriptor_id;
    DeviceDescriptor *device_descriptor;
    char text[QUEUE_MESSAGE_MESSAGE_LENGTH];

    device_pid = manual_control_get_device_pid(device_id);

    if (device_pid == -1) {
        return;
    }

    if (device_pid == 0) {
        println_color(COLOR_RED, "\tCannot find a device with id %d ",
                      device_id);
        return;
    }

    snprintf(text, 64, "%d\n%s\n%s\n", getpid(), switch_label, switch_pos);
    out_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER,
                                    QUEUE_MESSAGE_TYPE_DEVICE_START + device_id, text, true);

    in_message = queue_message_send_message_with_ack(device_pid, out_message);

    char **fields = device_communication_split_message_fields(in_message->mesg_text);

    descriptor_id = converter_string_to_long(fields[0]);

    device_descriptor = device_is_supported_by_id(descriptor_id.data.Long);

    if (device_descriptor == NULL) {
        println_color(COLOR_RED, "\tSet On Command: Device with unknown Device Descriptor id %ld",
                      descriptor_id.data.Long);
    }
    print("\t[%3ld] %-*s ", device_id, DEVICE_NAME_LENGTH,
          (device_descriptor == NULL) ? "?" : device_descriptor->name);
    if (strcmp(fields[1], MESSAGE_RETURN_SUCCESS) == 0) {
        print_color(COLOR_GREEN, "Switched ");
        print("'%s'", switch_label);
        print_color(COLOR_GREEN, " to ");
        println("'%s'", switch_pos);
    } else {
        snprintf(text, 64, "%s\n%s", fields[1], fields[2]);
        if (strcmp(text, MESSAGE_RETURN_NAME_ERROR) == 0) {
            println_color(COLOR_RED, "<label> %s doesn't exist",
                          switch_label);
        } else if (strcmp(text, MESSAGE_RETURN_VALUE_ERROR) == 0) {
            println_color(COLOR_RED, "<pos> %s doesn't exist",
                          switch_pos);
        } else if (strcmp(text, MESSAGE_RETURN_VALUE_PASSED_DATE_ERROR) == 0) {
            println_color(COLOR_RED, "The inserted date has already passsed");
        } else if (strcmp(text, MESSAGE_RETURN_VALUE_ORDER_DATE_ERROR) == 0) {
            println_color(COLOR_RED, "Please insert the dates in the right order");
        } else if (strcmp(text, MESSAGE_RETURN_VALUE_FORMAT_DATE_ERROR) == 0) {
            println_color(COLOR_RED, "Date format not valid");
        } else if (strcmp(text, MESSAGE_RETURN_VALUE_ALREADY_DEFINED_DATE_ERROR) == 0) {
            println_color(COLOR_RED, "Timer values already defined");
        } else if (strcmp(text, MESSAGE_RETURN_VALUE_SAME_DATE_ERROR) == 0) {
            println_color(COLOR_RED, "The two dates should be different");
        } else if (strcmp(text, MESSAGE_RETURN_VALUE_EXCEEDED_FRIDGE_ERROR) == 0) {
            println_color(COLOR_RED, "Maximum fridge capacity reached");
        } else if (strcmp(text, MESSAGE_RETURN_VALUE_EMPTY_FRIDGE_ERROR) == 0) {
            println_color(COLOR_RED, "Fridge is empty");
        } else if (strcmp(text, MESSAGE_RETURN_VALUE_MAXTHERMO_FRIDGE_ERROR) == 0) {
            println_color(COLOR_RED, "Cannot set internal temperature : too high");
        } else if (strcmp(text, MESSAGE_RETURN_VALUE_MINTHERMO_FRIDGE_ERROR) == 0) {
            println_color(COLOR_RED, "Cannot set internal temperature : too low");
        } else {
            println_color(COLOR_RED, "Unknown Error");
        }
    }
}