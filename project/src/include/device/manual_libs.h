#ifndef _MANUAL_CONTROL_LIBS_H
#define _MANUAL_CONTROL_LIBS_H

#include <stdbool.h>
#include <string.h>
#include "util/util_converter.h"
#include "device/device_communication.h"

#define DOMUS_GET_PID_SLEEP 250000

/**
 * Check if the specified pid is domus
 * @param pid the pid you want to check
 * @return true if is domus, false otherwise
 */
bool manual_control_check_domus(pid_t pid);

/**
 * Get device pid knowing its id
 * @param device_id the device id
 * @return the pid of the device
 */
pid_t manual_control_get_device_pid(size_t device_id);

/**
 * Set the devic with id device_id switch named switch_label to position switch_pos
 * @param device_id the device id
 * @param switch_label the switch name
 * @param switch_pos the value of the switch
 */
void manual_control_set_device(size_t device_id, char * switch_label, char * switch_pos);
#endif
