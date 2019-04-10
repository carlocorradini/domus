#ifndef _DOMUS_H
#define _DOMUS_H

#define DOMUS_VERSION "1.0.0"
#define DOMUS_LICENSE "MIT"
#define DOMUS_SLOGAN "Unicuique sua domus nota"
#define DOMUS_DESCRIPTION "Home Automation at your CLI"

/**
 * Domus Start Point
 */
void domus_start(void);

/**
 * Initialize all Domus Components
 */
static void domus_init(void);

/**
 * Free all Domus Components
 */
static void domus_free(void);

/**
 * Domus Welcome Page
 */
static void domus_welcome(void);

/**
 * Show information about Domus
 */
void domus_information(void);

#endif //_DOMUS_H
