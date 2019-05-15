#ifndef _PRINTER_H
#define _PRINTER_H

#include <stdio.h>
#include <stdbool.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD "\x1b[1m"
#define COLOR_INVERSE "\x1b[7m"
#define COLOR_BLACK "\x1b[30m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE   "\x1b[37m"
#define BACKGROUND_COLOR_CYAN "\x1b[46m"
#define BACKGROUND_COLOR_YELLOW "\x1b[43m"
#define BACKGROUND_COLOR_WHITE "\x1b[37m"

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

#endif
