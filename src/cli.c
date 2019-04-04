
#include "cli.h"

// ------------------------------------------------------------

int cli_help(char **args);

int cli_info(char **args);

int cli_exit(char **args);

int cli_num_commands();

const static char *CLI_COMMANDS[] = {
        "help",
        "info",
        "exit"
};

int (*CLI_FUNCTIONS[])(char **) = {
        &cli_help,
        &cli_info,
        &cli_exit
};

int cli_help(char **args) {
    int i;
    print(COLOR_BLUE, "Supported Commands {%d}\n", cli_num_commands());
    for (i = 0; i < cli_num_commands(); i++) {
        printf("\t%s\n", CLI_COMMANDS[i]);
    }
    return CLI_CONTINUE;
}

int cli_info(char **args) {
    domus_information();
    return CLI_CONTINUE;
}

int cli_exit(char **args) {
    return CLI_TERMINATE;
}

int cli_num_commands() {
    return sizeof(CLI_COMMANDS) / sizeof(char *);
}

// ------------------------------------------------------------

void cli_start(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("%s ", CLI_POINTER);
        line = cli_read_line();
        args = cli_split_line(line);
        status = cli_execute(args);

        free(line);
        free(args);
    } while (status);
}

static int cli_execute(char **args) {
    int i;

    if (args[0] == NULL) {
        // Nessun comando, CONTINUE
        return CLI_CONTINUE;
    }

    for (i = 0; i < cli_num_commands(); ++i) {
        if (strcmp(args[0], CLI_COMMANDS[i]) == 0) {
            return (*CLI_FUNCTIONS[i])(args);
        }
    }

    print(COLOR_RED, "NO COMMAND FOUND\n");
}

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