
#ifndef _DEVICE_H
#define _DEVICE_H

#include <stdbool.h>
#include <unistd.h>

typedef struct Device {
    pid_t PID;
    bool state;
    bool master_switch;
    unsigned int registry;
} Device;

#endif
