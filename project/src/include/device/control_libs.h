#ifndef _MANUAL_CONTROL_LIBS_H
#define _MANUAL_CONTROL_LIBS_H

#include <stdbool.h>
#include "util/util_converter.h"
#include "device/device_communication.h"

bool manual_control_check_domus(__pid_t domus_pid);

#endif
