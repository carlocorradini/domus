#ifndef _DEVICE_FRIDGE_H
#define _DEVICE_FRIDGE_H

#include "device/device.h"

#define DEVICE_FRIDGE_DEFAULT_DOOR false
#define DEVICE_FRIDGE_DEFAULT_TEMP 4.0
#define DEVICE_FRIDGE_DEFAULT_DELAY 16.0
#define DEVICE_FRIDGE_DEFAULT_PERC 50.0

/**
 * The Registry of the Fridge
 */
typedef struct FridgeRegistry {
    time_t time;
    long delay;
    float perc;
    double temp;
} FridgeRegistry;

/**
 * Create and return a new Fridge Registry
 * @return The new Fridge Registry, NULL otherwise
 */
FridgeRegistry *new_fridge_registry(void);

#endif

