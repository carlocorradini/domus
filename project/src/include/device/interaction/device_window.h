#ifndef _DEVICE_WINDOW_H
#define _DEVICE_WINDOW_H

#include <unistd.h>

#define WINDOW_SWITCH_OPEN "open"
#define WINDOW_SWITCH_OPEN_ON "on"
#define WINDOW_SWITCH_OPEN_OFF "off"

/**
 * The registry of the Window
 */
typedef struct WindowRegistry {
    double open;
} WindowRegistry;

/**
 * Create and return a new Window Registry
 * @return The new Window Registry, NULL otherwise
 */
WindowRegistry *new_window_registry(void);

#endif
