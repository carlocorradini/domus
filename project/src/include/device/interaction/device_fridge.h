#ifndef _DEVICE_FRIDGE_H
#define _DEVICE_FRIDGE_H

#include "device/device.h"

#define DEVICE_FRIDGE_DEFAULT_TEMP 4.0

typedef unsigned short int perc_t;
/**
 * The Registry of the Fridge
 */
typedef struct FridgeRegistry {
    time_t time;
    time_t delay;
    perc_t perc;
    double temp;
} FridgeRegistry;

/**
 * Create and return a new Fridge Registry
 * @return The new Fridge Registry, NULL otherwise
 */
FridgeRegistry *new_fridge_registry(void);

#endif

