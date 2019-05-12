#include "device/control_libs.h"

static __pid_t domus_pid = 0;

bool manual_control_check_domus(__pid_t pid){
    ConverterResult in_pid;
    Queue_message * check_message;
    Message * in_message;
    char text[QUEUE_MESSAGE_MESSAGE_LENGTH];

    snprintf(text, 64, "%d", getpid());
    check_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER, QUEUE_MESSAGE_TYPE_DOMUS_PID_REQUEST, text, true);

    in_message = queue_message_send_message_with_ack(pid, check_message);

    if(in_message != NULL) {
        in_pid = converter_string_to_long(in_message->mesg_text);

        if (in_pid.error || in_pid.data.Long != pid) {
            return false;
        }

        domus_pid = pid;
        return true;
    }
    return  false;
}

__pid_t manual_control_get_device_pid(size_t device_id){
    if(domus_pid == 0){
        fprintf(stderr, "\tPlease connect to domus first\n");
        return 0;
    }
    ConverterResult out_pid;
    Queue_message * check_message;
    Message * in_message;
    char text[QUEUE_MESSAGE_MESSAGE_LENGTH];

    snprintf(text, 64, "%lu\n%d\n", device_id, getpid());
    check_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER, QUEUE_MESSAGE_TYPE_PID_REQUEST, text, true);

    in_message = queue_message_send_message_with_ack(domus_pid, check_message);

    out_pid = converter_string_to_long(in_message->mesg_text);

    if(out_pid.error){
        return 0;
    }
    return out_pid.data.Long;
}

bool manual_control_set_device(size_t device_id, char * switch_name, char * switch_pos){
    __pid_t device_pid;
    Queue_message * out_message;
    Message * in_message;
    char text[QUEUE_MESSAGE_MESSAGE_LENGTH];

    device_pid = manual_control_get_device_pid(device_id);

    if(device_pid == 0){
        return false;
    }

    snprintf(text, 64, "%d\n%s\n%s\n", getpid(), switch_name, switch_pos);
    out_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER, QUEUE_MESSAGE_TYPE_DEVICE_START + device_id, text, true);

    in_message = queue_message_send_message_with_ack(device_pid, out_message);

    fprintf(stderr, "\tReceived : %s\n", in_message->mesg_text);
    if(strcmp(in_message->mesg_text, QUEUE_MESSAGE_RETURN_SUCCESS) == 0){
        return true;
    }
    return false;
}