
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "author.h"

Author *new_author(char name[], char surname[], char email[]) {
    Author *author = (Author *) malloc(sizeof(Author));
    if (!author) {
        perror("Author Memory Allocation");
        exit(EXIT_FAILURE);
    }

    strncpy(author->name, name, AUTHOR_NAME_LENGTH);
    strncpy(author->surname, surname, AUTHOR_SURNAME_LENGTH);
    strncpy(author->email, email, AUTHOR_EMAIL_LENGTH);
    return author;
}

void free_author(Author *author) {
    free(author->name);
    free(author->surname);
    free(author->email);
    free(author);
}

List *author_get_all(void) {
    List *authors = list_create(NULL);

    list_push(authors, new_author("Carlo", "Corradini", "carlo.corradini@studenti.unitn.it"));
    list_push(authors, new_author("Simone", "Nascivera", "simone.nascivera@studenti.unitn.it"));

    return authors;
}

void author_print_all(const List *authors) {
    Node *curr = *authors;
    Node *next;

    while (curr != NULL) {
        next = curr->next;
        author_print(curr->data);
        curr = next;
    }
}

void author_print(const Author *author) {
    printf("\t%-10s %-10s | %-10s\n", author->name, author->surname, author->email);
}