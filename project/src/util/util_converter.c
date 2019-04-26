
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include "util/util_converter.h"

ConverterResult converter_char_to_int(const char *char_string) {
    ConverterResult result = converter_char_to_long(char_string);
    result.data.Int = (int) result.data.Long;
    return result;
}

ConverterResult converter_char_to_long(const char *char_string) {
    ConverterResult result;
    const char *toRtn_str;
    char *toRtn_str_end = NULL;

    result.error = true;
    toRtn_str = char_string;
    toRtn_str_end = NULL;
    errno = 0;
    result.data.Long = strtol(toRtn_str, &toRtn_str_end, 10);

    if (toRtn_str == toRtn_str_end) {
        strncpy(result.error_message, "No digits found", CONVERTER_RESULT_ERROR_LENGTH);
    } else if (errno == ERANGE && result.data.Long == LONG_MIN) {
        strncpy(result.error_message, "Underflow", CONVERTER_RESULT_ERROR_LENGTH);
    } else if (errno == ERANGE && result.data.Long == LONG_MAX) {
        strncpy(result.error_message, "Overflow", CONVERTER_RESULT_ERROR_LENGTH);
    } else if (errno == EINVAL) {
        strncpy(result.error_message, "Base contains unsupported value", CONVERTER_RESULT_ERROR_LENGTH);
    } else if (errno != 0 && result.data.Long == 0) {
        strncpy(result.error_message, "Unspecified error occurred", CONVERTER_RESULT_ERROR_LENGTH);
    } else if (errno == 0 && toRtn_str && *toRtn_str_end != 0) {
        strncpy(result.error_message, "Additional characters remain", CONVERTER_RESULT_ERROR_LENGTH);
    } else {
        result.error = false;
    }

    return result;
}

ConverterResult converter_char_to_bool(const char *char_string) {
    ConverterResult result = converter_char_to_int(char_string);

    if (!result.error && (result.data.Int < false || result.data.Int > true)) {
        result.error = true;
        strncpy(result.error_message, "Not a valid boolean value", CONVERTER_RESULT_ERROR_LENGTH);
    }
    result.data.Bool = (bool) result.data.Int;

    return result;
}