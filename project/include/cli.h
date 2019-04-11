#ifndef _CLI_H
#define _CLI_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "domus.h"

#define CLI_POINTER '>'
#define CLI_CONTINUE 1
#define CLI_TERMINATE 0
#define CLI_QUESTION "?"
#define CLI_NEW_LINE '\n'
#define CLI_STRING_TERMINATOR '\0'
#define CLI_READ_LINE_BUFFER_SIZE 1024
#define CLI_SPLIT_LINE_BUFFER_SIZE 64
#define CLI_SPLIT_LINE_TOKEN_DELIMITER " \t\r\n\a"

/**
 * CLI Start Point
 */
void cli_start(void);

/**
 * Execute the command passed in args[0] or CONTINUE if no command found or args[0] == NULL
 * @param args Argument command + params
 * @return CLI status code: 'CONTINUE' | 'TERMINATE'
 */
static int cli_execute(char **args);

/**
 * Read in sequence the current line and put every char into a buffer until:
 *  - 'NEW_LINE' return the buffer
 *  - 'EOF' terminate
 * @return Buffer
 */
static char *cli_read_line(void);

/**
 * Split the line in tokens and return an array of strings
 * @param line Buffer current line
 * @return Array of strings terminating with NULL
 */
static char **cli_split_line(char *line);

#endif //_CLI_H
