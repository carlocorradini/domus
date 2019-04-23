
#include <stdbool.h>

#define TRIE_ALPHABET 26
#define TRIE_ALPHABET_FIRST 'a'
#define TRIE_ALPHABET_TERMINATOR '\0'

/**
 *
 */
typedef struct Trie_node {
    int data;
    struct Trie_node *array[TRIE_ALPHABET];
} Trie_node;

/**
 *
 */
typedef struct Trie {
    Trie_node *root;

    bool (*equals)(const void *, const void *);

    void (*destroy)(void *);
} Trie;

/**
 *
 * @param destroy
 * @param equals
 * @return
 */
Trie *new_trie(void (*destroy)(void *), bool(*equals)(const void *, const void *));

/**
 *
 * @return
 */
Trie_node *new_trie_node(void);

/**
 *
 * @param trie
 * @param word
 * @param value
 * @return
 */
Trie *trie_insert(Trie *trie, char *word, int value);

/**
 *
 * @param node
 * @param tmp
 * @param dat
 * @return
 */
char *trie_find_possible(Trie_node *node, char *tmp, char *dat);

/**
 *
 * @param node
 * @param word
 * @param dat
 * @return
 */
char *trie_search(Trie_node *node, char *word, char *dat);