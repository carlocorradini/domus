
#include "author.h"

const Author *author_get(void) {
    const static Author authors[] = {
            {"Carlo",  "Corradini", "carlo.corradini@studenti.unitn.it"},
            {"Simone", "Nascivera", "simone.nascivera@studenti.unitn.it"}
    };
    return authors;
}

void author_print_all(void) {
    const Author *authors = author_get();
    int i;
    for (i = 0; i < AUTHOR_COUNT; ++i) {
        author_print(&authors[i]);
    }
}

void author_print(const Author *author) {
    printf("\t%-10s %-10s | %-10s\n", author->name, author->surname, author->email);
}