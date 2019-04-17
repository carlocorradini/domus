
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "collection/collection_list.h"
#include "author.h"
#include "printer.h"

/**
 * List of authors
 */
static List *authors = NULL;

void authors_init(void) {
    if (authors != NULL) return;
    authors = new_list(NULL, NULL);

    list_add_last(authors, new_author(192451, "Carlo", "Corradini", "carlo.corradini@studenti.unitn.it"));
    list_add_last(authors, new_author(193246, "Simone", "Nascivera", "simone.nascivera@studenti.unitn.it"));
}

void authors_free(void) {
    free_list(authors);
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
    Author *data;
    if (authors == NULL) return;

    list_for_each(data, authors) {
        author_print(data);
    }
}

void author_print(const Author *author) {
    if (author == NULL) return;
    println("\t%-6s %-9s | %-34s | %6d", author->name, author->surname, author->email, author->id);
}