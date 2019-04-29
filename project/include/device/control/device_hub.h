
#ifndef _DEVICE_HUB_H
#define _DEVICE_HUB_H

#include <unistd.h>

/**
 * The Registry of the Hub
 */
typedef struct HubRegistry {
    time_t start;
} HubRegistry;

/**
 * Create and return a new Hub Registry
 * @return The new Hub Registry, NULL otherwise
 */
HubRegistry *new_hub_registry(void);

#endif
