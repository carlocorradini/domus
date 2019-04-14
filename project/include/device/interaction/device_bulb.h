
#ifndef _BULB_H
#define _BULB_H

#include "device/device.h"
#include <unistd.h>

typedef struct Device Bulb;

Bulb* init_bulb(pid_t fid);

void bulb_life(Bulb* bulb);
#endif
