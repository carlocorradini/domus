
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
        println_color(COLOR_RED, "\tNO COMMAND FOUND");
        status = CLI_CONTINUE;
    }
    return status;
}

static char *cli_read_line(void) {
    int c;
    int position = 0;
    char dat[100] = "";
    int buffer_size = CLI_READ_LINE_BUFFER_SIZE;
    char *buffer = (char *) malloc(sizeof(char) * buffer_size);

    if (buffer == NULL) {
        perror("Read Line Memory Allocation");
        exit(EXIT_FAILURE);
    }
    if (cli_list == NULL) {
        fprintf(stderr, "Cli List has not been initialized\n");
        exit(EXIT_FAILURE);
    }

    system("stty raw");

    while (true) {

        c = getchar();

        switch (c) {
            case CLI_ASCII_END_OF_TEXT: {
                printf("\033[100D");
                exit(EXIT_SUCCESS);
            }
            case CLI_ASCII_TAB: {
                printf("\033[6D");
                buffer[position] = CLI_STRING_TERMINATOR;
                char *res = command_autocomplete_search(buffer, dat);
                if (res != NULL) {
                    printf("\033[100D");
                    printf("\033[2C");
                    strcpy(buffer, res);
                    printf("%s", buffer);
                    position = (int) strlen(buffer);
                } else {
                    printf("%s", buffer);
                }
                break;
            }
            case CLI_ASCII_CARRIAGE_RETURN: {
                buffer[position] = CLI_STRING_TERMINATOR;
                printf("\033[2D");
                printf("   ");
                printf("\033[2D\n");
                printf("\033[100D");
                system("stty cooked");

                /*
                int i = 0;
                for(i; i< strlen(buffer); i++){
                    printf("%d ", buffer[i]);
                }
                printf("\n");
                */

                char *tmp = (char *) malloc(sizeof(char) * 512);
                strcpy(tmp, buffer);
                list_add_first(cli_list, tmp);

                cli_node = cli_list->head;

                return buffer;
            }
            case 65:
                printf("\033[4D");
                printf("    ");
                printf("\033[4D");

                if (cli_node == NULL) {
                    cli_node = cli_list->head;
                }

                printf("\033[100D");
                printf("\033[2C");
                int i = 0;
                for (i; i < 100; i++) {
                    printf(" ");
                }
                printf("\033[%dD", 100);


                char *data;
                data = (char *) cli_node->data;
                printf("%s", data);
                strcpy(buffer, data);
                position = (int) strlen(data);
                cli_node = cli_node->next;


                /*
                char * data;

                list_for_each(data, cli_list){
                    printf("%s ", data);
                }
                 */

                break;

            case 66:
                printf("\033[4D");
                printf("    ");
                printf("\033[4D");
                printf("new");
                break;
                // Delete
            case CLI_ASCII_DELETE: {
                if (position > 0) {
                    printf("\033[3D");
                    printf("   ");
                    printf("\033[3D");
                    position--;
                } else {
                    printf("\033[2D");
                    printf("   ");
                    printf("\033[3D");
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