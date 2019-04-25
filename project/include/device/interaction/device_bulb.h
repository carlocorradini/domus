
#ifndef _DEVICE_BULB_H
#define _DEVICE_BULB_H

/**
 * The Registry of the bulb
 */
typedef struct BulbRegistry {
    time_t start;
} BulbRegistry;

/**
 * Create and return a new Bulb Registry
 * @return The new Bulb Registry, NULL otherwise
 */
BulbRegistry *new_bulb_registry(void);

/**
 * Change the state of the bulb
 * @param state The state to change to
 * @return true if the change was successful, false otherwise
 */
bool bulb_master_switch(bool state);

#endif
