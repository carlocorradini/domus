
#ifndef DEVICE_CHILD_H
#define DEVICE_CHILD_H

#include <stdbool.h>
#include "device/device.h"
#include "device/device_communication.h"

#define DEVICE_CHILD_ARGS_LENGTH 3

/**
 * Check child arguments if correspond to the minimum required macro DEVICE_CHILD_ARGS_LENGTH
 * @param argc The number of arguments
 * @param args The arguments
 * @return true if args is valid, false otherwise
 */
bool device_child_check_args(int argc, char **args);

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
 * Create and return a Device Communication like
 *  Only for child process!
 * @return The new Device Communication, NULL otherwise
 */
DeviceCommunication *device_child_new_device_communication();

#endif
