
#include <stdarg.h>
#include "color.h"

static void printer(FILE *stream, const char *color, const char *format, va_list args) {
    fprintf(stream, "%s", color);
    vfprintf(stream, format, args);
    fprintf(stream, "%s", COLOR_RESET);
}

void print_stream(FILE *stream, const char *color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printer(stream, color, format, args);
    va_end(args);
}

void print(const char *color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printer(stdout, color, format, args);
    va_end(args);
}