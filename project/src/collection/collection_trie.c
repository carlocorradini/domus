
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "collection/collection_trie.h"

Trie *new_trie(void (*destroy)(void *), bool(*equals)(const void *, const void *)) {
    Trie *trie = (Trie *) malloc(sizeof(Trie));
    if (trie == NULL) {
        perror("New Trie Memory Allocation");
        exit(EXIT_FAILURE);
    }

    trie->root = NULL;
    trie->equals = equals;
    trie->destroy = destroy;

    return trie;
}

Trie_node *new_trie_node(void) {
    Trie_node *trie_node = (Trie_node *) malloc(sizeof(Trie_node));
    if (trie_node == NULL) {
        perror("New Trie Node Memory Allocation");
        exit(EXIT_FAILURE);
    }
    return trie_node;
}

Trie *trie_insert(Trie *trie, char *word, int value) {
    size_t i;
    size_t len;
    if (trie == NULL) return trie;
    if (strlen(word) == 0) return trie;

    Trie_node *tmp;
    if (trie->root == NULL) {
        trie->root = new_trie_node();
    }

    tmp = trie->root;

    len = strlen(word);
    for (i = 0; i < len; ++i) {
        if (tmp->array[word[i] - TRIE_ALPHABET_FIRST] == NULL) {
            tmp->array[word[i] - TRIE_ALPHABET_FIRST] = new_trie_node();
        }
        tmp = tmp->array[word[i] - TRIE_ALPHABET_FIRST];
    }
    tmp->data = value;

    return trie;
}

char *trie_find_possible(Trie_node *node, char *tmp, char *dat) {
    unsigned short int i;
    char a[2];
    if (node == NULL) return NULL;
    strcpy(tmp, dat);

    for (i = 0; i < TRIE_ALPHABET; i++) {
        if (node->array[i] != NULL) {
            a[0] = TRIE_ALPHABET_FIRST + i;
            a[1] = TRIE_ALPHABET_TERMINATOR;
            strcat(dat, a);
            if (node->array[i]->data > 0) {
                return dat;
            }
            return trie_find_possible(node->array[i], tmp, dat);
        }
    }
    return NULL;
}

char *trie_search(Trie_node *node, char *word, char *dat) {
    char tmp[100];
    char a[2];

    if (strlen(word) == 0) {
        strcat(dat, "\0");
        strcpy(tmp, dat);
        return trie_find_possible(node, tmp, dat);
    }

    if (node->array[word[0] - TRIE_ALPHABET_FIRST] != NULL) {
        a[0] = word[0];
        a[1] = TRIE_ALPHABET_TERMINATOR;
        strcat(dat, a);
        return trie_search(node->array[word[0] - TRIE_ALPHABET_FIRST], word + 1, dat);
    }

    return NULL;
}