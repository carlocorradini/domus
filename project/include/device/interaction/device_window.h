#ifndef _DEVICE_WINDOW_H
#define _DEVICE_WINDOW_H

#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

/**
 * The Registry of the Window
 */
typedef struct WindowRegistry {
    time_t start;
} WindowRegistry;

typedef struct WindowSwitch{
    bool state;
    bool (*setFunction)(bool);
} WindowSwitch;


WindowSwitch *new_window_switch(void);

WindowRegistry *new_window_registry(void);

bool setState(bool value);

#endif
