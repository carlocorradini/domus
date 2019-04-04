#ifndef _DOMUS_H
#define _DOMUS_H

#include "author.h"
#include "color.h"
#include "cli.h"

#define DOMUS_VERSION "1.0.0"
#define DOMUS_LICENSE "MIT"
#define DOMUS_SLOGAN "Unicuique sua domus nota"
#define DOMUS_DESCRIPTION "Home Automation at your CLI"

/**
 * Start Point
 */
void domus_start(void);

/**
 * Welcome Page
 */
static void domus_welcome(void);

/**
 * Informazioni riguardanti 'Domus'
 */
void domus_information(void);

#endif //_DOMUS_H
