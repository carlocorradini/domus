
#include <stdarg.h>
#include "printer.h"

/**
 * Print the string to stream with the selected color and add a newline if newline is true
 * @param stream Stream pointer output
 * @param color The Color
 * @param format The String
 * @param newline Add a newline at the end
 * @param args Format tags
 */
static void printer(FILE *stream, const char *color, const char *format, bool newline, va_list args);
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