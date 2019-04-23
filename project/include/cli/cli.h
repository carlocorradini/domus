#ifndef _CLI_H
#define _CLI_H

#define CLI_POINTER ">"
#define CLI_CONTINUE 1
#define CLI_TERMINATE 0
#define CLI_QUESTION "?"
#define CLI_NEW_LINE '\n'
#define CLI_STRING_TERMINATOR '\0'
#define CLI_READ_LINE_BUFFER_SIZE 1024
#define CLI_SPLIT_LINE_BUFFER_SIZE 64
#define CLI_SPLIT_LINE_TOKEN_DELIMITER " \t\r\n\a"
#define CLI_ASCII_END_OF_TEXT 3
#define CLI_ASCII_TAB 9
#define CLI_ASCII_CARRIAGE_RETURN 13
#define CLI_ASCII_DELETE 127

/**
 * CLI Start Point
 */
void cli_start(void);

#endif
