#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Trie_node {
    int data;
    struct Trie_node *array[26];
} Trie_node;

typedef struct Trie{
    Trie_node * root;

    bool (*equals)(const void *, const void *);
    void (*destroy)(void *);
} Trie;

Trie *new_trie(void (*destroy)(void *), bool(*equals)(const void *, const void *));

Trie_node *new_trie_node(void);

Trie *insert(Trie *trie, char *word, int value);

char* find_possible(Trie_node *node, char *tmp, char *dat);

char* search(Trie_node *node, char *word, char *dat);