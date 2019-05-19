
#ifndef _UTIL_CONVERTER_H
#define _UTIL_CONVERTER_H

#include <stdbool.h>
#include <time.h>

#define CONVERTER_RESULT_ERROR_LENGTH 64
#define CONVERTER_DATA_STRING_LENGTH 32
#define CONVERTER_DATA_MAX_DATE_STRING_LENGTH 64

#define CONVERTER_DATE_FORMAT "%Y-%m-%d_%H:%M:%S"

typedef struct ConverterResult {
    bool error;
    char error_message[CONVERTER_RESULT_ERROR_LENGTH];
    union data {
        bool Bool;
        int Int;
        long Long;
        double Double;
        struct tm Date;
        char String[CONVERTER_DATA_STRING_LENGTH];
    } data;
} ConverterResult;

/**
 * Convert a String to int
 * @param char_string The String to convert
 * @return The Converter Result
 */
ConverterResult converter_string_to_int(const char *char_string);

/**
 * Convert a String to long
 * @param char_string The String to convert
 * @return The Converter Result
 */
ConverterResult converter_string_to_long(const char *char_string);

/**
 * Convert a char to bool
 * @param char_value The char value to convert
 * @return The Converter Result
 */
ConverterResult converter_char_to_bool(char char_value);

/**
 * Convert a bool to String
 * @param value The boolean value to convert
 * @return The Converter Result
 */
ConverterResult converter_bool_to_string(bool value);

/**
 * Convert a char to double
 * @param char_value The char value to convert
 * @return The Converter Result
 */
ConverterResult converter_string_to_double(const char *char_string);

/**
 * Convert a string into a date
 * @param char_string The String to convert
 * @return The Converter Result
 */
ConverterResult converter_string_to_date(const char *char_string);

/**
 * Convert a date into a string
 * @param date The String to convert
 * @return The Converter Result
 */
ConverterResult converter_date_to_string(struct tm *  date);
#endif
