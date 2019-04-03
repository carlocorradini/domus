#ifndef DOMUS_AUTHOR_H
#define DOMUS_AUTHOR_H

#include <stdio.h>

#define AUTHOR_NAME_LENGTH 50
#define AUTHOR_SURNAME_LENGTH 50
#define AUTHOR_EMAIL_LENGTH 50

typedef struct Author {
    char name[AUTHOR_NAME_LENGTH];
    char surname[AUTHOR_SURNAME_LENGTH];
    char email[AUTHOR_EMAIL_LENGTH];
} Author;

void author_print(Author const *const author) {
    printf("%s %s | %s\n", author->name, author->surname, author->email);
}

#endif //DOMUS_AUTHOR_H
