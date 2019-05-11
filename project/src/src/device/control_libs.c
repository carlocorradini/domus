#include "device/control_libs.h"

void asd(){}

bool manual_control_check_domus(__pid_t domus_pid){

    Queue_message * check_message;
    Message * in_message;
    char text[QUEUE_MESSAGE_MESSAGE_LENGTH];

    snprintf(text, 64, "%d", getpid());
    check_message = new_queue_message(QUEUE_MESSAGE_QUEUE_NAME, QUEUE_MESSAGE_QUEUE_NUMBER, QUEUE_MESSAGE_TYPE_PID_REQUEST, text, true);
    queue_message_send_message(check_message);

    //ill(domus_pid, DEVICE_COMMUNICATION_READ_QUEUE);
    //queue_message_notify(domus_pid);
    sleep(1);
    queue_message_notify(domus_pid);

    signal(DEVICE_COMMUNICATION_READ_QUEUE, asd);
    pause();

    in_message = queue_message_receive_message(check_message->message_id, true);

    fprintf(stderr, "\t%s\n", in_message->mesg_text);

    return true;
}