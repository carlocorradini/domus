#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
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

    if (!result.error && (result.data.Int != false || result.data.Int != true)) {
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

ConverterResult converter_string_to_date(const char *char_string) {
    ConverterResult result;
    char str_date[CONVERTER_DATA_MAX_DATE_STRING_LENGTH];
    const char *toRtn_str;
    char *toRtn_str_end = NULL;
    char *pch;
    int tmp;

    strncpy(str_date, char_string, CONVERTER_DATA_MAX_DATE_STRING_LENGTH);
    result.error = false;
    toRtn_str = str_date;
    toRtn_str_end = NULL;
    errno = 0;
    pch = strtok(str_date, " ,.-:");
    tmp = strtol(pch, &toRtn_str_end, 10);


    if (toRtn_str == toRtn_str_end) {
        result.error = true;
        strncpy(result.error_message, "Format", CONVERTER_RESULT_ERROR_LENGTH);
    } else {
        result.data.Date.tm_year = tmp - 1900; //get the year value
        tmp = strtol(strtok(NULL, " ,.-:"), &toRtn_str_end, 10);;
        result.data.Date.tm_mon = tmp - 1;  //get the month value
        tmp = strtol(strtok(NULL, " ,.-:"), &toRtn_str_end, 10);
        result.data.Date.tm_mday = tmp; //get the day value
        tmp = strtol(strtok(NULL, " ,.-:"), &toRtn_str_end, 10);
        result.data.Date.tm_hour = tmp - 1; //get the hour value
        tmp = strtol(strtok(NULL, " ,.-:"), &toRtn_str_end, 10);
        result.data.Date.tm_min = tmp; //get the min value
        tmp = strtol(strtok(NULL, " ,.-:"), &toRtn_str_end, 10);
        result.data.Date.tm_sec = tmp; //get the sec value

        if (difftime(mktime(&result.data.Date), time(NULL)) < 0) {
            result.error = true;
            strncpy(result.error_message, "Passed", CONVERTER_RESULT_ERROR_LENGTH);
        }
    }

    return result;
}

ConverterResult converter_date_to_string(time_t date){
    ConverterResult result;
    struct tm* tm_info;

    tm_info = localtime(&date);
    strftime(result.data.String, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    result.error = false;

    return result;
}
