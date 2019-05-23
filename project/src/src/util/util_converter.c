
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "util/util_converter.h"

ConverterResult converter_string_to_int(const char *char_string) {
    ConverterResult result = converter_string_to_long(char_string);
    result.data.Int = (int) result.data.Long;
    return result;
}

ConverterResult converter_string_to_long(const char *char_string) {
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

ConverterResult converter_char_to_bool(char char_value) {
    const char char_string[2] = {char_value, '\0'};
    ConverterResult result = converter_string_to_int(char_string);

    if (result.error && (result.data.Int != false || result.data.Int != true)) {
        result.error = true;
        strncpy(result.error_message, "Not a valid boolean value", CONVERTER_RESULT_ERROR_LENGTH);
    }

    result.data.Bool = (bool) result.data.Int;

    return result;
}

ConverterResult converter_bool_to_string(bool value) {
    ConverterResult result;

    result.error = false;
    (value)
    ? strncpy(result.data.String, "true", CONVERTER_DATA_STRING_LENGTH)
    : strncpy(result.data.String, "false", CONVERTER_DATA_STRING_LENGTH);

    return result;
}

ConverterResult converter_string_to_double(const char *char_string) {
    ConverterResult result;
    const char *toRtn_str;
    char *toRtn_str_end = NULL;

    result.error = true;
    toRtn_str = char_string;
    toRtn_str_end = NULL;
    errno = 0;
    result.data.Double = strtod(toRtn_str, &toRtn_str_end);

    if (toRtn_str == toRtn_str_end) {
        strncpy(result.error_message, "No digits found", CONVERTER_RESULT_ERROR_LENGTH);
    } else if (errno == ERANGE && result.data.Double == LONG_MIN) {
        strncpy(result.error_message, "Underflow", CONVERTER_RESULT_ERROR_LENGTH);
    } else if (errno == ERANGE && result.data.Double == LONG_MAX) {
        strncpy(result.error_message, "Overflow", CONVERTER_RESULT_ERROR_LENGTH);
    } else if (errno == EINVAL) {
        strncpy(result.error_message, "Base contains unsupported value", CONVERTER_RESULT_ERROR_LENGTH);
    } else if (errno != 0 && result.data.Double == 0) {
        strncpy(result.error_message, "Unspecified error occurred", CONVERTER_RESULT_ERROR_LENGTH);
    } else if (errno == 0 && toRtn_str && *toRtn_str_end != 0) {
        strncpy(result.error_message, "Additional characters remain", CONVERTER_RESULT_ERROR_LENGTH);
    } else {
        result.error = false;
    }

    return result;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

ConverterResult converter_string_to_date(const char *char_string) {
    ConverterResult result;
    result.error = false;
    errno = 0;
    char *check;

    char buf[255];
    struct tm *data = malloc(sizeof(struct tm));

    if (strptime(char_string, CONVERTER_DATE_FORMAT, data) == 0) {
        result.error = true;
        strncpy(result.error_message, "Format", CONVERTER_RESULT_ERROR_LENGTH);
        free(data);
        return result;
    }
    data->tm_isdst = 1;

    result.data.Date = *(data);
    if (difftime(mktime(&result.data.Date), time(NULL)) < 0) {
        result.error = true;
        strncpy(result.error_message, "Passed", CONVERTER_RESULT_ERROR_LENGTH);
        free(data);
        return result;
    }
    free(data);
    return result;
}

ConverterResult converter_date_to_string(struct tm *date) {
    ConverterResult result;

    strftime(result.data.String, 26, CONVERTER_DATE_FORMAT, date);
    result.error = false;

    return result;
}
