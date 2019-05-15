#ifndef _MANUAL_CONTROL_LIBS_H
#define _MANUAL_CONTROL_LIBS_H

#include <stdbool.h>
#include "util/util_converter.h"
#include "device/device_communication.h"
#include <string.h>

#define DOMUS_GET_PID_SLEEP 250000

bool manual_control_check_domus(__pid_t pid);

__pid_t manual_control_get_device_pid(size_t device_id);

void manual_control_set_device(size_t device_id, char * switch_label, char * switch_pos);
#endif
