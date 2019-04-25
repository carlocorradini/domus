
#ifndef _DEVICE_BULB_H
#define _DEVICE_BULB_H

/**
 *
 */
typedef struct BulbRegistry {
    time_t start;
} BulbRegistry;

/**
 *
 * @return
 */
BulbRegistry *new_bulb_registry(void);

/**
 *
 * @param state
 * @return
 */
bool bulb_master_switch(bool state);

/**
 *
 */
void bulb_read_pipe(int signal_number);

#endif
