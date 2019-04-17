
#ifndef _COLLECTION_LINKED_LIST_H
#define _COLLECTION_LINKED_LIST_H

#define list_for_each(data, list) \
    Node *node; \
    for(node = list->head, data = node->data; node != NULL; node = node->next, data = linked_list_node_data(node))

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    void *data;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct LinkedList {
    size_t size;
    Node *head;
    Node *tail;

    bool (*equals)(void *, void *);

    void (*destroy)(void *);
} LinkedList;

/**
 * Create a new List:
 *  *destroy can be NULL
 *  *equals can be NULL
 * @param destroy A function to destroy the element of the list
 * @param compare A function to compare two elements of the list
 * @return The new List
 */
LinkedList *new_linked_list(void (*destroy)(void *), bool(*equals)(void *, void *));

/**
 * Free a List
 * @param list The list to free
 * @return true if the list has been freed, false otherwise
 */
bool free_linked_list(LinkedList *list);

/**
 * Check if a List is empty or not
 * @param list The List to check
 * @return true if empty, false otherwise
 */
bool linked_list_is_empty(const LinkedList *list);

/**
 * Return the Node data
 * @param node The Node to get data from
 * @return The Node data, NULL otherwise
 */
void *linked_list_node_data(const Node *node);

/**
 * Inserts a Node at the specified at the specified position in the List
 * Shifts the element currently at that position (if any) and any subsequent elements to the right (adds one to their indices)
 * @param list The List to add a new Node
 * @param index The position to add a new Node
 * @param data The data of the Node
 * @return true if a Node has been added, false otherwise
 */
bool linked_list_add(LinkedList *list, size_t index, void *data);

/**
 * Inserts a Node at the beginning of this list
 * @param list The List to add a new Node
 * @param data The data of the Node
 * @return true if a Node has been added, false otherwise
 */
bool linked_list_add_first(LinkedList *list, void *data);

/**
 * Appends a Node at the end of this list
 * @param list The List to add a new Node
 * @param data The data of the Node
 * @return true if a Node has been added, false otherwise
 */
bool linked_list_add_last(LinkedList *list, void *data);

/**
 * Returns the Node at the specified position in the List
 * @param list The List to get from
 * @param index The position of the Node
 * @return The Node at the specified position, NULL otherwise
 */
void *linked_list_get(const LinkedList *list, size_t index);

/**
 * Returns the first element in this list
 * @param list The List to get from
 * @return The Node at the specified position, NULL otherwise
 */
void *linked_list_get_first(const LinkedList *list);

/**
 * Returns the last element in this list
 * @param list The List to get from
 * @return The Node at the specified position, NULL otherwise
 */
void *linked_list_get_last(const LinkedList *list);

/**
 * Removes the Node at the specified position in this list
 * @param list The List to remove from
 * @param index The index of the Node to be removed
 * @return The Node at the specified position, NULL otherwise
 */
void *linked_list_remove(LinkedList *list, size_t index);

/**
 * Removes and returns the first element from the List
 * @param list The List to remove from
 * @return The first element from the List
 */
void *linked_list_remove_first(LinkedList *list);

/**
 * Removes and returns the last element from the List
 * @param list The List to remove from
 * @return The last element from the List
 */
void *linked_list_remove_last(LinkedList *list);

/**
 * Returns true if this List contains the specified element.
 * More formally, returns true if and only if the List contains at least one element e such that
 * @param list The List to check
 * @param data The element to check
 * @return true if this List contains the specified element, false otherwise
 */
bool linked_list_contains(const LinkedList *list, void *data);

#endif
