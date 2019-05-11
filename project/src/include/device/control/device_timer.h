
#ifndef _DEVICE_TIMER_H
#define _DEVICE_TIMER_H

#include <unistd.h>

#define TIMER_SWITCH_TIME "time"
#define TIMER_DATE_DELIMITER "?"

/**
 * The Registry of the Timer
 */
typedef struct TimerRegistry {
    time_t begin;
    time_t end;
} TimerRegistry;

/**
 * Create and return a new Timer Registry
 * @return The new Timer Registry, NULL otherwise
 */
TimerRegistry *new_timer_registry(void);

#endif
