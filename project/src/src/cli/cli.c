
#include <string.h>
#include "cli/cli.h"
#include "cli/command/command.h"
#include "util/util_printer.h"
#include "collection/collection_list.h"
#include "collection/collection_trie.h"


static List *cli_list = NULL;
static Node *cli_node = NULL;

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

void cli_start(void) {
    char *line;
    char **args;
    int status;

    cli_list = new_list(NULL, NULL);

    do {
        print("%s ", CLI_POINTER);
        line = cli_read_line();
        args = cli_split_line(line);
        status = cli_execute(args);

        free(line);
        free(args);
    } while (status);
}

static int cli_execute(char **args) {
    int status = command_execute(args);
    if (status == -1) {
        /* No Command found, CONTINUE */
        println_color(COLOR_RED, "\tCommand '%s' not found", args[0]);
        status = CLI_CONTINUE;
    }
    return status;
}

/**
 * Move the cursor to the left
 * @param value # of positions (if null, it's set to 100)
 */
static void cursor_left(int value) {
    if (value != 0) {
        printf("\033[%dD", value);
    } else {
        printf("\033[100D");
    }
}

/**
 * Move the cursor to the right
 * @param value # of positions (if null, it's set to 100)
 */
static void cursor_right(int value) {
    if (value != 0) {
        printf("\033[%dC", value);
    } else {
        printf("\033[100C");
    }
}

/**
 * Print white spaces
 * @param value # of white spaces to print
 */
static void white_space(int value) {
    int i;
    for (i = 0; i < value; i++) {
        printf(" ");
    }
}

/**
 * "Delete" 1 character by getting last buffer postion and
 * deleting everything after that
 * @param position the buffer position
 */
static void clear_from_char(int position) {
    cursor_left(0);
    cursor_right(position + 2);
    white_space(7);
    cursor_left(7);
}

/**
 * Move cursor to the left right after the ">" character
 */
static void move_left() {
    cursor_left(0);
    cursor_right(2);
}

/**
 * Check if c is a number
 * @param c the char
 * @return return true/false
 */
static bool isNumber(char c) {
    return c > 47 && c < 58;
}

/**
 * Check if c is a capital letter
 * @param c the char
 * @return return true/false
 */
static bool isCapital(char c) {
    return c > 64 && c < 9;
}

/**
 * Check if c is a lowcase letter
 * @param c the char
 * @return return true/false
 */
static bool isLower(char c) {
    return c > 96 && c < 123;
}

static char *cli_read_line(void) {
    int c;
    int position = 0;
    char dat[CLI_READ_LINE_BUFFER_SIZE] = "";
    int buffer_size = CLI_READ_LINE_BUFFER_SIZE;
    char *buffer = (char *) malloc(sizeof(char) * buffer_size);

    if (buffer == 0) {
        perror("Read Line Memory Allocation");
        exit(EXIT_FAILURE);
    }
    if (cli_list == 0) {
        fprintf(stderr, "Cli List has not been initialized\n");
        exit(EXIT_FAILURE);
    }

    /*
     * switch to raw terminal mode in order
     * to catch special characters
     */
    system("stty raw");

    while (true) {

        c = getchar();

        if (isCapital(c) || isLower(c) || isNumber(c) || c == CLI_CHARACTER_DELETE ||
            c == CLI_CHARACTER_CARRIAGE_RETURN || c == CLI_CHARACTER_TAB || c == CLI_CHARACTER_ARROW ||
            c == CLI_CHARACTER_EXIT || c == CLI_CHARACTER_SPACE || c == CLI_CHARACTER_MINUS ||
            c == CLI_CHARACTER_QUESTION_MARK) {
            switch (c) {
                /*
                 * If Ctrl + C is typed
                 */
                case CLI_CHARACTER_EXIT: {
                    /*
                     * return the cursor to the most left column
                     */
                    clear_from_char(position);
                    printf("\n");
                    /*
                    * switch back to normal terminal
                    */
                    system("stty cooked");
                    strcpy(buffer, "exit");
                    return (buffer);
                }

                    /*
                     * If Tab is pressed
                     */
                case CLI_CHARACTER_TAB: {
                    /*
                     * Clear
                     */
                    clear_from_char(position);
                    /*
                     * "Close" the current buffer
                     */
                    buffer[position] = CLI_STRING_TERMINATOR;

                    strncpy(dat, "", CLI_READ_LINE_BUFFER_SIZE);
                    char *res = command_autocomplete_search(buffer, dat);
                    /*
                     * if exists a word with that prefix
                     */
                    if (res != NULL) {
                        move_left();
                        strncpy(buffer, res, sizeof(res));
                        strcat(buffer, "\0");
                        printf("%s", buffer);
                        position = (int) strlen(buffer);
                    }
                    break;
                }

                    /*
                     * if Enter is pressed
                     */
                case CLI_CHARACTER_CARRIAGE_RETURN: {
                    buffer[position] = CLI_STRING_TERMINATOR;
                    cursor_left(2);
                    white_space(3);
                    cursor_left(0);
                    printf("\n");
                    system("stty cooked");

                    /*
                     * Do not add empty buffer to list
                     */
                    if (strlen(buffer) > 0) {
                        char *tmp = (char *) malloc(sizeof(char) * 512);
                        strcpy(tmp, buffer);
                        list_add_first(cli_list, tmp);
                    }
                    cli_node = cli_list->head;

                    return buffer;
                }


                    /*
                     * if an Arrow is pressed
                     */
                case CLI_CHARACTER_ARROW: {

                    /*
                     * Check which arrow was pressed
                     */
                    c = getchar();

                    /*
                     * If is up arrow
                     */
                    if (c == CLI_CHARACTER_UP_ARROW) {


                        if (cli_list->head != NULL) {
                            if (cli_node == NULL) {
                                cli_node = cli_list->head;
                            }
                            move_left();
                            /*
                             * Clear everything
                             */
                            white_space(100);
                            cursor_left(100);

                            char *data;
                            data = (char *) cli_node->data;
                            printf("%s", data);
                            strcpy(buffer, data);
                            position = (int) strlen(data);
                            cli_node = cli_node->next;
                            if (cli_node == NULL) {
                                cli_node = cli_list->head;
                            }


                        }
                        break;
                    }

                    /*
                     * If is down arrow
                     */
                    if (c == CLI_CHARACTER_DOWN_ARROW) {
                        if (cli_node != NULL) {
                            if (cli_node->prev == NULL) {
                                cli_node = cli_list->tail;
                            } else {
                                cli_node = cli_node->prev;
                            }
                            if (cli_node->prev == NULL) {
                                cli_node = cli_list->tail;
                            } else {
                                cli_node = cli_node->prev;
                            }
                            move_left();
                            /*
                             * Clear everything
                             */
                            white_space(100);
                            cursor_left(100);

                            char *data;
                            data = (char *) cli_node->data;
                            printf("%s", data);
                            strcpy(buffer, data);
                            position = (int) strlen(data);
                            cli_node = cli_node->next;
                            if (cli_node == NULL) {
                                cli_node = cli_list->head;
                            }
                        }
                        break;
                    }
                    break;
                }

                    /*
                     * If Delete was pressed
                     */
                case CLI_CHARACTER_DELETE: {
                    /*
                     * If it's after ">" character
                     */
                    if (position > 0) {
                        cursor_left(3);
                        white_space(3);
                        cursor_left(3);
                        position--;
                    } else {
                        cursor_left(2);
                        white_space(3);
                        cursor_left(3);
                    }
                    break;
                }

                default: {
                    if (c == EOF) {
                        exit(EXIT_SUCCESS);
                    } else {

                        buffer[position] = c;
                        position++;
                    }
                }
            }
        } else {
            clear_from_char(position);
        }
    }
}


static char **cli_split_line(char *line) {
    int position = 0;
    int buffer_size = CLI_SPLIT_LINE_BUFFER_SIZE;
    char *token;
    char **buffer = (char **) malloc(sizeof(char *) * buffer_size);

    if (buffer == NULL) {
        perror("Split Line Memory Allocation");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, CLI_SPLIT_LINE_TOKEN_DELIMITER);
    while (token != NULL) {
        buffer[position] = token;
        position++;

        /* Buffer dimension reached, reallocation */
        if (position >= buffer_size) {
            buffer_size += CLI_SPLIT_LINE_BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size * sizeof(char *));
            if (buffer == NULL) {
                perror("Split Line Memory Allocation");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, CLI_SPLIT_LINE_TOKEN_DELIMITER);
    }

    buffer[position] = NULL;
    return buffer;
}