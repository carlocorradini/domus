#ifndef _LIST_H
#define _LIST_H

#define list_for_each(item, list) \
    Node *item = *list; \
    for(item; item != NULL; item = item->next)

/**
 * Node Struct
 */
typedef struct Node {
    void *data;
    struct Node *next;
} Node;

typedef Node *List;

/**
 * Create a List
 * @param data Initial element
 * @return The list
 */
List *list_create(void *data);

/**
 * Free a List
 * @param list The list to be free
 */
void list_free(List *list);

/**
 * Add an element at the front of the list
 * @param list The list
 * @param data The element to push
 */
void list_push(List *list, void *data);

/**
 * Removes and returns the first element of this list
 * @param list The list
 * @return The element at the front of the list
 */
void *list_pop(List *list);

#endif
