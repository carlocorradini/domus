#include "device/manual_control.h"
#include "device/device_communication.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "string.h"
#include "util/util_converter.h"
#include "cli/cli.h"
#include "cli/command/command.h"


int main() {
    manual_command_init();
    cli_start();
    /**
     * Clean current pipe and create a new one
     */
     /*
    Queue_message *out_message;

    char buffer[QUEUE_MESSAGE_MESSAGE_LENGTH];
    int device = 1;
    snprintf(buffer, QUEUE_MESSAGE_MESSAGE_LENGTH, "%ld\n%ld\n", device, (long) getpid());
    printf("Mt PID answer : %d \n", getpid());
    out_message = new_queue_message("control", 1, QUEUE_MESSAGE_TYPE_PID_REQUEST, buffer, true);

    queue_message_send_message(out_message);

    signal(SIGUSR2, test1);
    pause();

    ConverterResult domus;

    Message * in_message;
    in_message = queue_message_receive_message(out_message->message_id, true);

    domus = converter_string_to_long(in_message->mesg_text);
    domus_pid = domus.data.Long;

    printf("Device is :%d\n", domus_pid);
    queue_message_remove_message_queue(out_message->message_id);

    char device_name[4];
    snprintf(device_name, 2, "%d", device);
    Queue_message * a = new_queue_message(device_name, device, QUEUE_MESSAGE_TYPE_PID_REQUEST, "ciao", true);
    queue_message_send_message(a);
    sleep(3);
    kill(domus_pid, SIGUSR2);
      */
    return 0;
}
