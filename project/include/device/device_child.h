
#ifndef _DEVICE_CHILD_H
#define _DEVICE_CHILD_H

#include <stdbool.h>
#include "device/device.h"
#include "device/device_communication.h"

#define DEVICE_CHILD_ARGS_LENGTH 4

/**
 * An endless loop with pause for low LEVEL CPU LOAD
 * @param do_on_wake_up A function called when the process has woke up
 */
void device_child_run(void (*do_on_wake_up)(void));

/**
 * Create and return a Device like but with arguments parameters.
 *  Only for child process!
 * @param argc The number of arguments
 * @param args The arguments
 * @param registry Device registry
 * @return The new Device, NULL otherwise
 */
Device *device_child_new_device(int argc, char **args, void *registry);

/**
 * Create and return a Device Communication like,
 *  For easy of use, pass a message handler function
 *  Only for child process!
 * @param argc The number of arguments
 * @param args The arguments
 * @param message_handler The message handler function for child
 * @return The new Device Communication, NULL otherwise
 */
DeviceCommunication *
device_child_new_device_communication(int argc, char **args, void (*message_handler)(DeviceCommunicationMessage));

/**
 * Create and return a Control Device like but with arguments parameters
 * @param argc The number of arguments
 * @param args The arguments
 * @param registry Device registry
 * @return The new Control Device, NULL otherwise
 */
ControlDevice *device_child_new_control_device(int argc, char **args, void *registry);

/**
 * Create and return a Device Communication like,
 *  For easy of use, pass a message handler function
 *  Only for child process!
 *  Only for Control Devices!
 * @param argc The number of arguments
 * @param args The arguments
 * @param message_handler The message handler function for child
 * @return The new Device Communication, NULL otherwise
 */
DeviceCommunication *device_child_new_control_device_communication(int argc, char **args,
                                                                   void (*message_handler)(DeviceCommunicationMessage));

#endif
