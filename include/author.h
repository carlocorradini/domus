#ifndef _AUTHOR_H
#define _AUTHOR_H

#include <stdio.h>

#define AUTHOR_COUNT 2
#define AUTHOR_NAME_LENGTH 50
#define AUTHOR_SURNAME_LENGTH 50
#define AUTHOR_EMAIL_LENGTH 50

/**
 * Struct Autore
 */
typedef struct Author {
    char name[AUTHOR_NAME_LENGTH];
    char surname[AUTHOR_SURNAME_LENGTH];
    char email[AUTHOR_EMAIL_LENGTH];
} Author;

/**
 * Ritorna un array di Author
 * @return Array di Author
 */
const Author *author_get(void);

/**
 * Stampa tutti gli autori utilizzando la sintassi definita in 'author_print'
 */
void author_print_all(void);

/**
 * Stampa le informazioni di un autore in 'stdout'
 * @param author Autore da stampare
 */
void author_print(const Author *author);

#endif //_AUTHOR_H
