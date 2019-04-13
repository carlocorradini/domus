
#include "include/cli/cli.h"
#include "printer.h"
#include "command/command.h"

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
        println_color(COLOR_RED, "\tNO COMMAND FOUND");
        status = CLI_CONTINUE;
    }
    return status;
}

static char *cli_read_line(void) {
    int c;
    int position = 0;
    int buffer_size = CLI_READ_LINE_BUFFER_SIZE;
    char *buffer = (char *) malloc(sizeof(char) * buffer_size);

    if (buffer == NULL) {
        perror("Read Line Memory Allocation");
        exit(EXIT_FAILURE);
    }

    while (true) {
        c = getchar();

        if (c == EOF) {
            exit(EXIT_SUCCESS);
        } else if (c == CLI_NEW_LINE) {
            buffer[position] = CLI_STRING_TERMINATOR;
            return buffer;
        } else {
            buffer[position] = c;
        }

        position++;

        /* Buffer dimension reached, reallocation */
        if (position >= buffer_size) {
            buffer_size += CLI_READ_LINE_BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (buffer == NULL) {
                perror("Read Line Memory Allocation");
                exit(EXIT_FAILURE);
            }
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