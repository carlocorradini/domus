
#ifndef DEVICE_CHILD_H
#define DEVICE_CHILD_H

#include <stdbool.h>
#include "device/device.h"
#include "device/device_communication.h"

#define DEVICE_CHILD_ARGS_LENGTH 3

/**
 * Create and return a Device like but with arguments parameters.
 *  Only for child process!
 * @param argc The number of arguments
 * @param args The arguments
 * @param registry Device registry
 * @param master_switch Function to change state and make custom operations
 * @return The new Device, NULL otherwise
 */
Device *device_child_new_device(int argc, char **args, void *registry, bool (*master_switch)(bool));

/**
 * Create and return a Device Communication like,
 *  For easy of use, pass a message handler function
 *  Only for child process!
 * @param message_handler The message handler function for child
 * @return The new Device Communication, NULL otherwise
 */
DeviceCommunication *device_child_new_device_communication(void (*message_handler)(DeviceCommunicationMessage));

#endif
