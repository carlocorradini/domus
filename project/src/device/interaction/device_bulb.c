
#include <limits.h>
#include <errno.h>
#include <string.h>
#include "device/device_communication.h"
#include "device/interaction/device_bulb.h"

int main(int argc, char **args) {

    DeviceCommunicationMessage message;
    char *end;
    pid_t pid = strtol(args[0], &end, 10);

    if (pid > INT_MAX || pid < INT_MIN || errno == ERANGE) {
        perror("Conversion error");
        exit(EXIT_FAILURE);
    }

    int i;
    for (i = 0; i < 10; ++i) {
        message.type = i;
        strncpy(message.message, "TEST DI MESSAGGIO ", DEVICE_COMMUNICATION_MESSAGE_LENGTH);
        strcat(message.message, args[0]);
        device_communication_write(DEVICE_COMMUNICATION_CHILD_WRITE, &message);
    }
    device_communication_notify(pid);


    return 0;
}