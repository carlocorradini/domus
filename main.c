#include <stdlib.h>
#include "color.h"

int main() {
    printf("WELCOME TO DOMUS\n");
    print(COLOR_BLUE, "Blue\n");
    print(COLOR_CYAN, "Cyan\n");
    print(COLOR_GREEN, "Green\n");
    print(COLOR_MAGENTA, "Magenta\n");
    print(COLOR_RED, "Red\n");
    print(COLOR_YELLOW, "Yellow\n");
    print(COLOR_WHITE, "White\n");
    print_stream(stdout, COLOR_BLUE, "Stampo in stdout con %d parametri: %c | %c", 2, 'A', 'B');
    return EXIT_SUCCESS;
}