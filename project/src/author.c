
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "author.h"
#include "printer.h"

List *authors = NULL;

void authors_init(void) {
    if (authors != NULL) return;
    authors = list_create(NULL);

    list_push(authors, new_author(192451, "Carlo", "Corradini", "carlo.corradini@studenti.unitn.it"));
    list_push(authors, new_author(193246, "Simone", "Nascivera", "simone.nascivera@studenti.unitn.it"));
}

void authors_free(void) {
    list_free(authors);
}

Author *new_author(unsigned int id, char name[], char surname[], char email[]) {
    Author *author = (Author *) malloc(sizeof(Author));
    if (author == NULL) {
        perror("Author Memory Allocation");
        exit(EXIT_FAILURE);
    }

    author->id = id;
    strncpy(author->name, name, AUTHOR_NAME_LENGTH);
    strncpy(author->surname, surname, AUTHOR_SURNAME_LENGTH);
    strncpy(author->email, email, AUTHOR_EMAIL_LENGTH);
    return author;
}

void author_print_all() {
    Author *author;
    if (authors == NULL) return;

    list_for_each(item, authors)
    {
        author = (Author *) item->data;
        author_print(author);
    }
}

void author_print(const Author *author) {
    if (author == NULL) return;
    println("\t%-6s %-9s | %-34s | %6d", author->name, author->surname, author->email, author->id);
}