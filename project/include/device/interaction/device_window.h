#ifndef _DEVICE_WINDOW_H
#define _DEVICE_WINDOW_H

/**
 * The registry of the Window
 */
typedef struct WindowRegistry {
    time_t open;
} WindowRegistry;

/**
 * Create and return a new Window Registry
 * @return The new Window Registry, NULL otherwise
 */
WindowRegistry *new_window_registry(void);

#endif