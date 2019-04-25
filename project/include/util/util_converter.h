
#ifndef UTIL_CONVERTER_H
#define UTIL_CONVERTER_H

#include <stdbool.h>

/**
 * Convert a String to int
 * @param char_string The String to convert
 * @return The int value
 */
int converter_char_to_int(const char *char_string);

/**
 * Convert a String to bool
 * @param char_string The String to convert
 * @return The bool value
 */
bool converter_char_to_bool(const char *char_string);

#endif
