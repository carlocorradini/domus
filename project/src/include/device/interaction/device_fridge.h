#ifndef _DEVICE_FRIDGE_H
#define _DEVICE_FRIDGE_H

#include "device/device.h"

#define DEVICE_FRIDGE_MAX_ITEM 128
#define DEVICE_FRIDGE_MAX_THERMO 20
#define DEVICE_FRIDGE_MIN_THERMO -273.15
#define DEVICE_FRIDGE_DEFAULT_DOOR false
#define DEVICE_FRIDGE_DEFAULT_TEMP 4.0
#define DEVICE_FRIDGE_DEFAULT_DELAY 16.0
#define DEVICE_FRIDGE_DEFAULT_CAPACITY_ITEM 0
#define DEVICE_FRIDGE_DEFAULT_CAPACITY_PERC ((float)DEVICE_FRIDGE_DEFAULT_CAPACITY_ITEM/DEVICE_FRIDGE_MAX_ITEM)*100

#define FRIDGE_SWITCH_DOOR "door"
#define FRIDGE_SWITCH_DOOR_ON "on"
#define FRIDGE_SWITCH_DOOR_OFF "off"
#define FRIDGE_SWITCH_STATE "state"
#define FRIDGE_SWITCH_STATE_ON "on"
#define FRIDGE_SWITCH_STATE_OFF "off"
#define FRIDGE_SWITCH_THERMO "thermo"
#define FRIDGE_SWITCH_DELAY "delay"
#define FRIDGE_SWITCH_FILLING "filling"
/**
 * The Registry of the Fridge
 */
typedef struct FridgeRegistry {
    time_t time;
    long delay;
    float perc;
    double temp;
    long items;
} FridgeRegistry;

/**
 * Create and return a new Fridge Registry
 * @return The new Fridge Registry, NULL otherwise
 */
FridgeRegistry *new_fridge_registry(void);

#endif

