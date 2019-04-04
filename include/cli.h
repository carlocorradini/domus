#ifndef _CLI_H
#define _CLI_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "domus.h"

#define CLI_POINTER ">"
#define CLI_CONTINUE 1
#define CLI_TERMINATE 0
#define CLI_NEW_LINE '\n'
#define CLI_STRING_TERMINATOR '\0'
#define CLI_READ_LINE_BUFFER_SIZE 1024
#define CLI_SPLIT_LINE_BUFFER_SIZE 64
#define CLI_SPLIT_LINE_TOKEN_DELIMITER "\t\r\n\a"

/**
 * Start Point
 */
void cli_start(void);

/**
 * Esegue il comando con le variabili passate
 *  altrimenti 'CONTINUE'
 * @param args Argument command + tokens
 * @return CLI status code: 'CONTINUE' | 'TERMINATE'
 */
static int cli_execute(char **args);

/**
 * Legge in sequenza i caratteri della riga corrente fino al raggiungimento del caratter:
 *  - 'NEW_LINE' ritornando il buffer
 *  - 'EOF' terminando il programma
 * @return Buffer char riga corrente
 */
static char *cli_read_line(void);

/**
 * Splitta la linea in tokens e ritorna un buffer si token(stringhe)
 * @param line Buffer char riga corrente
 * @return Buffer stringhe tokens terminante con 'NULL'
 */
static char **cli_split_line(char *line);

#endif //_CLI_H
