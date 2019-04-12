
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "author.h"
#include "printer.h"

List *authors = NULL;

void authors_init(void) {
    if (authors) return;
    authors = list_create(NULL);

    list_push(authors, new_author("Carlo", "Corradini", "carlo.corradini@studenti.unitn.it"));
    list_push(authors, new_author("Simone", "Nascivera", "simone.nascivera@studenti.unitn.it"));
}

/*
void authors_free(void) {
    if (!authors) return;

    Author *tmp;
    list_for_each(item, authors){
        tmp = (Author*) item->data;
        free_author(tmp);
    }
}
*/

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

//would be nice to implement
/*
void free_author(Author *author) {
    if (!author) return;
    free(author->name);
    printf("\nhere name\n");
    println("%s", author->surname);
    free(author->surname);
    printf("\nhere surname\n");

    //free(author->email);
    printf("\nhere email\n");

    //free(author);
    printf("\nhere author\n");
}
*/

void author_print_all() {
    if (!authors) return;
    Node *curr = *authors;
    Node *next;

    while (curr != NULL) {
        next = curr->next;
        author_print(curr->data);
        curr = next;
    }
}

void author_print(const Author *author) {
    if (!author) return;
    println("\t%-10s %-10s | %-10s", author->name, author->surname, author->email);
}