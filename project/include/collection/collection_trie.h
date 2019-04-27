
#ifndef _COLLECTION_TRIE_H
#define _COLLECTION_TRIE_H

#include <stdbool.h>

#define TRIE_ALPHABET 26
#define TRIE_ALPHABET_FIRST 'a'
#define TRIE_ALPHABET_TERMINATOR '\0'

/**
 *  Trie data structure
 *  data is 0 when no words terminate with that node
 *  array is an array of Trie_nodes composed by 26 character (a = 0)
 */
typedef struct Trie_node {
    int data;
    struct Trie_node *array[TRIE_ALPHABET];
} Trie_node;

/**
 *  Trie is basically a Trie_node (the root one)
 *  and optionally implements the equals and destroy methods
 */
typedef struct Trie {
    Trie_node *root;

    bool (*equals)(const void *, const void *);

    void (*destroy)(void *);
} Trie;

/**
 *  Create new Trie
 * @param destroy
 * @param equals
 * @return the trie
 */
Trie *new_trie(void (*destroy)(void *), bool(*equals)(const void *, const void *));

/**
 * Create new trie_node
 * @return
 */
Trie_node *new_trie_node(void);

/**
 *  Insert a new word in the trie
 * @param trie the trie to insert the value to
 * @param word the word to insert
 * @param value (>0) the value for the corrisponding word
 * @return the modified trie
 */
Trie *trie_insert(Trie *trie, char *word, int value);

/**
 *  Check if a word with "word" prefix exists in trie.
 *  If not, return NULL otherwise return the word itself
 * @param node the starting trie_node
 * @param word the prefix typed by the user
 * @param dat the partial result
 * @return
 */
char *trie_search(Trie_node *node, char *word, char *dat);

#endif