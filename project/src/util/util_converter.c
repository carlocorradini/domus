
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "util/util_converter.h"

int converter_char_to_int(const char *char_string) {
    return converter_char_to_long(char_string);
}

long converter_char_to_long(const char *char_string) {
    long toRtn;
    const char *toRtn_str;
    char *toRtn_str_end = NULL;

    toRtn_str = char_string;
    toRtn_str_end = NULL;
    errno = 0;
    toRtn = strtol(toRtn_str, &toRtn_str_end, 10);

    if (toRtn_str == toRtn_str_end) {
        fprintf(stderr, "Conversion Error: No digits found\n");
    } else if (errno == EINVAL) {
        fprintf(stderr, "Conversion Error: Base contains unsupported value\n");
    } else if (errno != 0 && toRtn == 0) {
        fprintf(stderr, "Conversion Error: Unspecified error occurred\n");
    } else if (errno == 0 && toRtn_str && *toRtn_str_end != 0) {
        fprintf(stderr, "Conversion Error: Additional characters remain\n");
    }

    return toRtn;
}

bool converter_char_to_bool(const char *char_string) {
    int toRtn = converter_char_to_int(char_string);

    if (toRtn < false || toRtn > true) {
        fprintf(stderr, "Conversion Error: Character is not a boolean value\n");
    }

    return toRtn;
}