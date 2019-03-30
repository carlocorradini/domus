#ifndef DOMUS_ANSI_H
#define DOMUS_ANSI_H

#include <stdarg.h>
#include <stdio.h>

#define ANSI_COLOR_RESET   "\x1b[0m"
// COLORS
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"

static void printer(FILE *stream, const char *color, const char *format, va_list args) {
    printf("%s", color);
    vfprintf(stream, format, args);
    printf("%s", ANSI_COLOR_RESET);
}

/**
 *
 * @param stream
 * @param color
 * @param format
 * @param ...
 */
void print_stream(FILE *stream, const char *color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printer(stream, color, format, args);
    va_end(args);
}

/**
 *
 * @param color
 * @param format
 * @param ...
 */
void print(const char *color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printer(stdout, color, format, args);
    va_end(args);
}

#endif //DOMUS_ANSI_H
