#ifndef DOMUS_CLI_H
#define DOMUS_CLI_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "domus.h"

#define CLI_CONTINUE 1
#define CLI_TERMINATE 0
#define CLI_NEW_LINE '\n'
#define CLI_STRING_TERMINATOR '\0'
#define CLI_READ_LINE_BUFFER_SIZE 1024
#define CLI_SPLIT_LINE_BUFFER_SIZE 64
#define CLI_SPLIT_LINE_TOKEN_DELIMITER "\t\r\n\a"

void domus_print_information(void);

int lsh_help(char **args) {
    domus_print_information();

    return CLI_CONTINUE;
}

int lsh_exit(char **args) {
    return CLI_TERMINATE;
}

char *CLI_COMMANDS[] = {
        "help",
        "exit"
};

int (*CLI_FUNCTIONS[])(char **) = {
        &lsh_help,
        &lsh_exit
};

int num_commands() {
    return sizeof(CLI_COMMANDS) / sizeof(char *);
}

/**
 * Legge in sequenza i caratteri della riga corrente fino al raggiungimento del caratter:
 *  - 'NEW_LINE' ritornando il buffer
 *  - 'EOF' terminando il programma
 * @return Buffer char riga corrente
 */
static char *cli_read_line(void) {
    int c;
    int position = 0;
    int buffer_size = CLI_READ_LINE_BUFFER_SIZE;
    char *buffer = malloc(sizeof(char) * buffer_size);

    if (!buffer) {
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

        // Dimensione del Buffer raggiunta, rialloco memoria
        if (position >= buffer_size) {
            buffer_size += CLI_READ_LINE_BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                perror("Read Line Memory Allocation");
                exit(EXIT_FAILURE);
            }
        }
    }
}

/**
 * Splitta la linea in tokens e ritorna un buffer si token(stringhe)
 * @param line Buffer char riga corrente
 * @return Buffer stringhe tokens terminante con 'NULL'
 */
static char **cli_split_line(char *line) {
    int position = 0;
    int buffer_size = CLI_SPLIT_LINE_BUFFER_SIZE;
    char *token;
    char **tokens = malloc(sizeof(char *) * buffer_size);
    char **tokens_backup;

    if (!tokens) {
        perror("Split Line Memory Allocation");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, CLI_SPLIT_LINE_TOKEN_DELIMITER);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        // Dimensione del Buffer raggiunta, rialloco memoria
        if (position >= buffer_size) {
            buffer_size += CLI_SPLIT_LINE_BUFFER_SIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, buffer_size * sizeof(char *));

            if (!tokens) {
                free(tokens_backup);
                perror("Split Line Memory Allocation");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, CLI_SPLIT_LINE_TOKEN_DELIMITER);
    }

    tokens[position] = NULL;
    return tokens;
}

/**
 * Esegue il comando con le variabili passate
 *  altrimenti 'CONTINUE'
 * @param args Argument command + tokens
 * @return CLI status code: 'CONTINUE' | 'TERMINATE'
 */
static int cli_execute(char **args) {
    int i;

    if (args[0] == NULL) {
        // Nessun comando, CONTINUE
        return CLI_CONTINUE;
    }

    for (i = 0; i < num_commands(); ++i) {
        if (strcmp(args[0], CLI_COMMANDS[i]) == 0) {
            return (*CLI_FUNCTIONS[i])(args);
        }
    }

    print(COLOR_RED, "NO COMMAND FOUND\n");
}

void cli_start(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = cli_read_line();
        args = cli_split_line(line);
        status = cli_execute(args);

        free(line);
        free(args);
    } while (status);
}

#endif //DOMUS_CLI_H
