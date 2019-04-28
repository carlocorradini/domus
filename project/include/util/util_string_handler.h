#ifndef _UTIL_STRING_HANDLER_H
#define _UTIL_STRING_HANDLER_H

/**
 * String to string array converter
 * @param string input string
 * @return char** tokenized_string
 */
char **string_to_string_array(const char string[]);

/**
 * String array to string converter
 * @param input pointer of strings
 * @return string with MESSAGE_DELIMETER between each
 */
char *string_array_to_string(const char *input[]);
#endif