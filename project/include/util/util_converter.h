
#ifndef UTIL_CONVERTER_H
#define UTIL_CONVERTER_H

#include <stdbool.h>

#define CONVERTER_RESULT_ERROR_LENGTH 64

typedef struct ConverterResult {
    bool error;
    char error_message[CONVERTER_RESULT_ERROR_LENGTH];
    union data {
        bool Bool;
        int Int;
        long Long;
    } data;
} ConverterResult;

/**
 * Convert a String to int
 * @param char_string The String to convert
 * @return The Converter Result
 */
ConverterResult converter_char_to_int(const char *char_string);

/**
 * Convert a String to long
 * @param char_string The String to convert
 * @return The Converter Result
 */
ConverterResult converter_char_to_long(const char *char_string);

/**
 * Convert a String to bool
 * @param char_string The String to convert
 * @return The Converter Result
 */
ConverterResult converter_char_to_bool(const char *char_string);

#endif
