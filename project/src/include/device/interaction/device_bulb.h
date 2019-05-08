
#ifndef _DEVICE_BULB_H
#define _DEVICE_BULB_H

#include <unistd.h>

/**
 * The Registry of the Bulb
 */
typedef struct BulbRegistry {
    double _time;
} BulbRegistry;

/**
 * Create and return a new Bulb Registry
 * @return The new Bulb Registry, NULL otherwise
 */
BulbRegistry *new_bulb_registry(void);

#endif
