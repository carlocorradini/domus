
#include "domus.h"

void domus_start(void) {
    domus_welcome();
    cli_start();
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
    );
    printf("%s\n\n", DOMUS_SLOGAN);
    domus_information();
    printf("=================================================================\n");
}

void domus_information(void) {
    print(COLOR_BLUE, "-> INFORMATION <-\n");
    print(COLOR_MAGENTA, "- AUTHORS\n");
    author_print_all();
    print(COLOR_MAGENTA, "- DESCRIPTION\n");
    printf("\t%s\n", DOMUS_DESCRIPTION);
    print(COLOR_MAGENTA, "- VERSION\n");
    printf("\t%s\n", DOMUS_VERSION);
    print(COLOR_MAGENTA, "- LICENSE\n");
    printf("\t%s\n", DOMUS_LICENSE);
}
