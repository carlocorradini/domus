#ifndef DOMUS_DOMUS_H
#define DOMUS_DOMUS_H

#include "author.h"
#include "color.h"
#include "cli.h"

#define DOMUS_VERSION "1.0.0"
#define DOMUS_DESCRIPTION "Unicuique sua domus nota"
#define DEBUG

static Author authors[] = {
        {"Carlo",  "Corradini", "carlo.corradini@studenti.unitn.it"},
        {"Simone", "Nascivera", "simone.nascivera@studenti.unitn.it"}
};

void domus_print_information(void) {
    int i;

    print(COLOR_BLUE, "-> INFORMATION <-\n");
    print(COLOR_MAGENTA, "- AUTHORS -\n");
    for (i = 0; i < 2; ++i) {
        author_print(&authors[i]);
    }
    print(COLOR_MAGENTA, "- DESCRIPTION -\n");
    printf("%s\n", DOMUS_DESCRIPTION);
    print(COLOR_MAGENTA, "- VERSION -\n");
    printf("%s\n", DOMUS_VERSION);
}

static void domus_welcome(void) {
    printf("=================================================================\n");
    printf(
            " _______    ______   .___  ___.  __    __       _______.\n"
            "|       \\  /  __  \\  |   \\/   | |  |  |  |     /       |\n"
            "|  .--.  ||  |  |  | |  \\  /  | |  |  |  |    |   (----`\n"
            "|  |  |  ||  |  |  | |  |\\/|  | |  |  |  |     \\   \\    \n"
            "|  '--'  ||  `--'  | |  |  |  | |  `--'  | .----)   |   \n"
            "|_______/  \\______/  |__|  |__|  \\______/  |_______/    \n"
            "\n"
    );
    domus_print_information();
    printf("=================================================================\n");
}

void domus_start(void) {
    domus_welcome();
    cli_start();
}

#endif //DOMUS_DOMUS_H
