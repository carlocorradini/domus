
#include <stdarg.h>
#include "printer.h"

static void printer(FILE *stream, const char *color, const char *format, bool newline, va_list args) {
    fprintf(stream, "%s", color);
    vfprintf(stream, format, args);
    fprintf(stream, "%s", COLOR_RESET);
    if (newline) fprintf(stream, "\n");
}

void print(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printer(stdout, COLOR_WHITE, format, false, args);
    va_end(args);
}

void print_color(const char *color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printer(stdout, color, format, false, args);
    va_end(args);
}

void println(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printer(stdout, COLOR_WHITE, format, true, args);
    va_end(args);
}

void println_color(const char *color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printer(stdout, color, format, true, args);
    va_end(args);
}