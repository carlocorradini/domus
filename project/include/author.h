#ifndef _AUTHOR_H
#define _AUTHOR_H

#include "collection/collection_list.h"

#define AUTHOR_NAME_LENGTH 50
#define AUTHOR_SURNAME_LENGTH 50
#define AUTHOR_EMAIL_LENGTH 50

/**
 * Struct Author
 */
typedef struct Author {
    unsigned int id;
    char name[AUTHOR_NAME_LENGTH];
    char surname[AUTHOR_SURNAME_LENGTH];
    char email[AUTHOR_EMAIL_LENGTH];
} Author;

/**
 * Collection of Authors
 */
extern List *authors;

/**
 * Initialize the List of Authors
 */
void authors_init(void);

/**
 * Free the List of Authors
 */
void authors_free(void);

/**
 * Create a new Author
 * @param id Identifier
 * @param name Author name
 * @param surname Author surname
 * @param email Author email
 * @return The new Author
 */
Author *new_author(unsigned int id, char name[], char surname[], char email[]);

/**
 * Print all Author to stdout
 * @param authors The list of Author
 */
void author_print_all();

/**
 * Print an Author in stdout
 * @param author The author to print
 */
void author_print(const Author *author);

#endif
