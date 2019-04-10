#ifndef _PRINTER_H
#define _PRINTER_H

#include <stdio.h>
#include <stdbool.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE    "\x1b[37m"

/**
 * Print the string to stream with the selected color and add a newline if newline is true
 * @param stream Stream pointer output
 * @param color The Color
 * @param format The String
 * @param newline Add a newline at the end
 * @param args Format tags
 */
static void printer(FILE *stream, const char *color, const char *format, bool newline, va_list args);

/**
 * Print the string to stdout
 * @param format The String
 * @param ... Format tags
 */
void print(const char *format, ...);

/**
 * Print the string to stdout with selected color
 * @param color The Color
 * @param format The String
 * @param ... Format tags
 */
void print_color(const char *color, const char *format, ...);

/**
 * Print the string to stdout and add a newline
 * @param format The String
 * @param ... Format tags
 */
void println(const char *format, ...);

/**
 * Print the string to stdout with selected color and add a newline
 * @param color The Color
 * @param format The String
 * @param ... Format tags
 */
void println_color(const char *color, const char *format, ...);

#endif //_PRINTER_H
