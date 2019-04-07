#ifndef _AUTHOR_H
#define _AUTHOR_H

#define AUTHOR_NAME_LENGTH 50
#define AUTHOR_SURNAME_LENGTH 50
#define AUTHOR_EMAIL_LENGTH 50

#include "data_structure/list.h"

/**
 * Struct Author
 */
typedef struct Author {
    char name[AUTHOR_NAME_LENGTH];
    char surname[AUTHOR_SURNAME_LENGTH];
    char email[AUTHOR_EMAIL_LENGTH];
} Author;

/**
 * Create a new Author
 * @param name Author name
 * @param surname Author surname
 * @param email Author email
 * @return The new Author
 */
Author *new_author(char name[], char surname[], char email[]);

/**
 * Free an Author
 * @param author The author to free
 */
void free_author(Author *author);

/**
 * Get the List of Author
 * @return The List of Author
 */
List *author_get_all(void);

/**
 * Print all Autor in stdout
 * @param authors The list of Author
 */
void author_print_all(const List *authors);

/**
 * Print an Author in stdout
 * @param author The author to print
 */
void author_print(const Author *author);

#endif //_AUTHOR_H
