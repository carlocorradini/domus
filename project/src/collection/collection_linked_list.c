
#include "collection/collection_linked_list.h"

/**
 * Check if 'index' is a valid index
 * @param list The List to check with
 * @param index The index to check
 * @return true if is a valid index, false otherwise
 */
static bool linked_list_check_index(const LinkedList *list, size_t index);

/**
 * Create a new non empty Node with next and prev NULL
 * @param data The data of the Node
 * @return The new Node
 */
static Node *linked_list_new_node(void *data);

/**
 * Free a Node returning it's data
 * @param node The Node to free
 * @return The data of the Node
 */
static void *linked_list_free_node(Node *node);

/**
 * Return a Node of the List or NULL if List is empty or index is invalid
 * @param list The List to get from
 * @param index The index of the Node
 * @return The Node of the List at index
 */
static Node *linked_list_get_node(const LinkedList *list, size_t index);

LinkedList *new_linked_list(void (*destroy)(void *), bool(*equals)(void *, void *)) {
    LinkedList *list = (LinkedList *) malloc(sizeof(LinkedList));
    if (list == NULL) {
        perror("New Linked List Memory Allocation");
        exit(EXIT_FAILURE);
    }

    list->size = 0;
    list->head = list->tail = NULL;
    list->equals = equals;
    list->destroy = destroy;

    return list;
}

bool free_linked_list(LinkedList *list) {
    Node *node;
    Node *next;
    if (list == NULL) return false;

    if (!linked_list_is_empty(list)) {
        node = list->head;
        while (node != NULL) {
            next = node->next;
            if (list->destroy == NULL) {
                free(linked_list_free_node(node));
            } else {
                list->destroy(linked_list_free_node(node));
            }
            node = next;
        }
    }
    free(list);

    return true;
}

bool linked_list_is_empty(const LinkedList *list) {
    if (list == NULL) return NULL;
    return list->size == 0 && list->head == NULL && list->tail == NULL;
}

void *linked_list_node_data(const Node *node) {
    if(node == NULL) return NULL;
    return node->data;
}

static bool linked_list_check_index(const LinkedList *list, size_t index) {
    if (list == NULL) return false;
    return index >= 0 && index <= list->size;
}

static Node *linked_list_new_node(void *data) {
    Node *node = (Node *) malloc(sizeof(Node));
    if (node == NULL) {
        perror("New Node Linked List Memory Allocation");
        exit(EXIT_FAILURE);
    }

    node->data = data;
    node->next = node->prev = NULL;

    return node;
}

static void *linked_list_free_node(Node *node) {
    void *data = node->data;
    free(node);
    return data;
}

static Node *linked_list_get_node(const LinkedList *list, size_t index) {
    Node *node;
    size_t i;
    if (list == NULL) return NULL;
    if (linked_list_is_empty(list) || !linked_list_check_index(list, index)) return NULL;

    node = list->head;
    for (i = 0; i < index; i++) {
        node = node->next;
    }

    return node;
}

bool linked_list_add(LinkedList *list, size_t index, void *data) {
    Node *new_node;
    Node *node;
    if (list == NULL) return false;
    if (!linked_list_check_index(list, index)) return false;

    if (index == 0) {
        return linked_list_add_first(list, data);
    }
    if (index == list->size - 1) {
        return linked_list_add_last(list, data);
    }

    new_node = linked_list_new_node(data);
    node = linked_list_get_node(list, index);

    new_node->prev = node->prev;
    new_node->next = node;
    new_node->prev->next = new_node;
    new_node->next->prev = new_node;
    list->size++;

    return true;
}

bool linked_list_add_first(LinkedList *list, void *data) {
    Node *node;
    if (list == NULL) return false;

    node = linked_list_new_node(data);
    node->next = list->head;

    if (list->head != NULL) {
        list->head->prev = node;
    }
    if (list->tail == NULL) {
        list->tail = node;
    }
    list->head = node;
    list->size++;

    return true;
}

bool linked_list_add_last(LinkedList *list, void *data) {
    Node *node;
    if (list == NULL) return false;

    node = linked_list_new_node(data);
    node->prev = list->tail;

    if (list->tail != NULL) {
        list->tail->next = node;
    }
    if (list->head == NULL) {
        list->head = node;
    }
    list->tail = node;
    list->size++;

    return true;
}

void *linked_list_get(const LinkedList *list, size_t index) {
    if (list == NULL) return NULL;
    if (linked_list_is_empty(list) || !linked_list_check_index(list, index)) return NULL;

    return linked_list_get_node(list, index)->data;
}

void *linked_list_get_first(const LinkedList *list) {
    return linked_list_get(list, 0);
}

void *linked_list_get_last(const LinkedList *list) {
    if (list == NULL) return NULL;
    return linked_list_get(list, list->size - 1);
}

void *linked_list_remove(LinkedList *list, size_t index) {
    Node *node;
    void *data = NULL;
    if (list == NULL) return NULL;
    if (linked_list_is_empty(list) || !linked_list_check_index(list, index)) return NULL;

    node = linked_list_get_node(list, index);

    if (node->prev == NULL && node->next == NULL) {
        list->head = list->tail = NULL;
    } else if (node->prev == NULL) {
        list->head = node->next;
        list->head->prev = NULL;
    } else if (node->next == NULL) {
        list->tail = node->prev;
        list->tail->next = NULL;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    data = linked_list_free_node(node);
    list->size--;
    return data;
}

void *linked_list_remove_first(LinkedList *list) {
    return linked_list_remove(list, 0);
}

void *linked_list_remove_last(LinkedList *list) {
    if (list == NULL) return NULL;
    return linked_list_remove(list, list->size - 1);
}

bool linked_list_contains(const LinkedList *list, void *data) {
    Node *node;
    if (list == NULL || data == NULL) return false;
    if (linked_list_is_empty(list)) return false;
    if (list->equals == NULL) {
        fprintf(stderr, "Linked List: Unable to compare, please define a valid equals function\n");
    }

    node = list->head;
    while (node != NULL && !list->equals(node->data, data)) {
        node = node->next;
    }

    return node != NULL;
}