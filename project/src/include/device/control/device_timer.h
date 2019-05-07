
#ifndef _DEVICE_TIMER_H
#define _DEVICE_TIMER_H

#include <unistd.h>

/**
 * The Registry of the Hub
 */
typedef struct TimerRegistry {
    time_t begin;
    time_t end;
} TimerRegistry;

/**
 * Create and return a new Hub Registry
 * @return The new Hub Registry, NULL otherwise
 */
TimerRegistry *new_timer_registry(void);

#endif
