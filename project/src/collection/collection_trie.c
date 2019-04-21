#include "collection/collection_trie.h"


Trie *new_trie(void (*destroy)(void *), bool(*equals)(const void *, const void *)) {
    Trie *trie = (Trie *) malloc(sizeof(Trie));
    if (trie == NULL) {
        perror("New List Memory Allocation");
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
        perror("New List Memory Allocation");
        exit(EXIT_FAILURE);
    }
    return trie_node;
}

bool trie_is_empty(const Trie *trie) {
    if (trie == NULL) {
        return true;
    }
    return false;
}

bool trie_node_is_empty(const Trie_node *trie) {
    if (trie == NULL) {
        return true;
    }
    return false;
}

Trie *insert(Trie *trie, char *word, int value) {

    if (strlen(word) == 0)
        return trie;

    if (trie == NULL) {
        return trie;
    }

    Trie_node *tmp;
    if (trie->root == NULL) {
        trie->root = new_trie_node();
    }

    tmp = trie->root;

    size_t i;
    size_t len = strlen(word);

    for (i = 0; i < len; ++i) {
        if(tmp->array[word[i] - 'a'] == NULL){
            tmp->array[word[i] - 'a'] = new_trie_node();
        }
        tmp = tmp->array[word[i] - 'a'];
    }
    tmp->data = value;

    return trie;
}

char* find_possible(Trie_node *node, char *tmp, char *dat){

    int i=0;
    strcpy(tmp, dat);
    for(i; i<26; i++){
        if(node->array[i] != NULL){
            char a[2];
            a[0] = 'a' + i;
            a[1] = '\0';
            strcat(dat, a);
            if(node->array[i]->data > 0){
                return dat;
            }
            return find_possible(node->array[i], tmp, dat);
        }
    }
    return NULL;
}

char* search(Trie_node *node, char *word, char *dat){
    if(strlen(word) == 0){
        char tmp[100];
        strcat(dat, "\0");
        strcpy(tmp, dat);
        return find_possible(node, tmp, dat);
    } else{
        if(node->array[word[0] - 'a'] != NULL){
            char a[2];
            a[0] = word[0];
            a[1] = '\0';
            strcat(dat, a);
            return search(node->array[word[0] - 'a'], word+1, dat);
        } else{
            return NULL;
        }
    }

}