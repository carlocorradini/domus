#ifndef DOMUS_COLOR_H
#define DOMUS_COLOR_H

#include <stdarg.h>
#include <stdio.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE    "\x1b[37m"

/**
 * Stampa nello `stream` la stringa con colore ANSI selezionato
 * @param stream Stream pointer output
 * @param color Colore di stampa
 * @param format Stringa di testo da scrivere nello stream
 * @param args Format tags da sostituire nel testo | OPTIONAL
 */
static void printer(FILE *stream, const char *color, const char *format, va_list args) {
    printf("%s", color);
    vfprintf(stream, format, args);
    printf("%s", COLOR_RESET);
}

/**
 * Stampa nello `stream` la stringa con colore ANSI selezionato
 * @param stream Stream pointer output
 * @param color Colore di stampa
 * @param format Stringa di testo da scrivere nello stream
 * @param ... Format tags da sostituire nel testo | OPTIONAL
 */
void print_stream(FILE *stream, const char *color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printer(stream, color, format, args);
    va_end(args);
}

/**
 * Stampa in `stdout` la stringa con colore ANSI selezionato
 * @param color Colore di stampa
 * @param format Stringa di testo da scrivere nello stream
 * @param ... Format tags da sostituire nel testo | OPTIONAL
 */
void print(const char *color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printer(stdout, color, format, args);
    va_end(args);
}

#endif //DOMUS_COLOR_H
