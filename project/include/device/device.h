
#ifndef _DEVICE_H
#define _DEVICE_H

#include <stdbool.h>

typedef struct Device {
    bool state;
    bool master_switch;
    unsigned int registry;
} Device;

#endif
