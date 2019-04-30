
#include "collection/collection_list.h"

/**
 * Check if 'index' is a valid index
 * @param list The List to check with
 * @param index The index to check
 * @return true if is a valid index, false otherwise
 */
static bool list_check_index(const List *list, size_t index);

/**
 * Create a new non empty Node with next and prev NULL
 * @param data The data of the Node
 * @return The new Node
 */
static Node *list_new_node(void *data);

/**
 * Free a Node returning it's data
 * @param node The Node to free
 * @return The data of the Node
 */
static void *list_free_node(Node *node);

/**
 * Return a Node of the List or NULL if List is empty or index is invalid
 * @param list The List to get from
 * @param index The index of the Node
 * @return The Node of the List at index
 */
static Node *list_get_node(const List *list, size_t index);

List *new_list(void (*destroy)(void *), bool(*equals)(const void *, const void *)) {
    List *list = (List *) malloc(sizeof(List));
    if (list == NULL) {
        perror("New List Memory Allocation");
        exit(EXIT_FAILURE);
    }

    list->size = 0;
    list->head = list->tail = NULL;
    list->equals = equals;
    list->destroy = destroy;

    return list;
}

bool free_list(List *list) {
    Node *node;
    Node *next;
    if (list == NULL) return false;

    if (!list_is_empty(list)) {
        node = list->head;
        while (node != NULL) {
            next = node->next;
            if (list->destroy == NULL) {
                free(list_free_node(node));
            } else {
                list->destroy(list_free_node(node));
            }
            node = next;
        }
    }
    free(list);

    return true;
}

bool list_is_empty(const List *list) {
    if (list == NULL) return true;
    return list->size == 0 && list->head == NULL && list->tail == NULL;
}

void *list_node_data(const Node *node) {
    if (node == NULL) return NULL;
    return node->data;
}

static bool list_check_index(const List *list, size_t index) {
    if (list == NULL) return false;
    return index <= list->size;
}

static Node *list_new_node(void *data) {
    Node *node = (Node *) malloc(sizeof(Node));
    if (node == NULL) {
        perror("New Node List Memory Allocation");
        exit(EXIT_FAILURE);
    }

    node->data = data;
    node->next = node->prev = NULL;

    return node;
}

static void *list_free_node(Node *node) {
    void *data = node->data;
    free(node);
    return data;
}

static Node *list_get_node(const List *list, size_t index) {
    Node *node;
    size_t i;
    if (list == NULL) return NULL;
    if (list_is_empty(list) || !list_check_index(list, index)) return NULL;

    node = list->head;
    for (i = 0; i < index; i++) {
        node = node->next;
    }

    return node;
}


bool list_add(List *list, size_t index, void *data) {
    Node *new_node;
    Node *node;
    if (list == NULL) return false;
    if (!list_check_index(list, index)) return false;

    if (index == 0) {
        return list_add_first(list, data);
    }
    if (index == list->size - 1) {
        return list_add_last(list, data);
    }

    new_node = list_new_node(data);
    node = list_get_node(list, index);

    new_node->prev = node->prev;
    new_node->next = node;
    new_node->prev->next = new_node;
    new_node->next->prev = new_node;
    list->size++;

    return true;
}

bool list_add_first(List *list, void *data) {
    Node *node;
    if (list == NULL) return false;

    node = list_new_node(data);

    if (list->head != NULL) {
        node->next = list->head;
        list->head->prev = node;
    }
    if (list->tail == NULL) {
        list->tail = node;
    }
    list->head = node;
    list->size++;

    return true;
}

bool list_add_last(List *list, void *data) {
    Node *node;
    if (list == NULL) return false;

    node = list_new_node(data);

    if (list->tail != NULL) {
        node->prev = list->tail;
        list->tail->next = node;
    }
    if (list->head == NULL) {
        list->head = node;
    }
    list->tail = node;
    list->size++;

    return true;
}

void *list_get(const List *list, size_t index) {
    if (list == NULL) return NULL;
    if (list_is_empty(list) || !list_check_index(list, index)) return NULL;

    return list_get_node(list, index)->data;
}

size_t list_get_index(const List *list, const void *data) {
    Node *node;
    size_t index;
    if (list == NULL || data == NULL) return false;
    if (list_is_empty(list)) return false;
    if (list->equals == NULL) {
        fprintf(stderr, "List: Unable to compare, please define a valid equals function\n");
        return false;
    }

    node = list->head;
    index = 0;
    while (node != NULL && !list->equals(node->data, data)) {
        node = node->next;
        index++;
    }

    if (node != NULL) return index;
    return -1;
}

void *list_get_first(const List *list) {
    if (list == NULL) return NULL;
    return list_get(list, 0);
}

void *list_get_last(const List *list) {
    if (list == NULL) return NULL;
    return list_get(list, list->size - 1);
}

void *list_remove_index(List *list, size_t index) {
    Node *node;
    void *data = NULL;
    if (list == NULL) return NULL;
    if (list_is_empty(list) || !list_check_index(list, index)) return NULL;

    node = list_get_node(list, index);

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

    data = list_free_node(node);
    list->size--;
    return data;
}

void *list_remove_first(List *list) {
    return list_remove_index(list, 0);
}

void *list_remove_last(List *list) {
    if (list == NULL) return NULL;
    return list_remove_index(list, list->size - 1);
}

bool list_remove(List *list, const void *data) {
    Node *node;
    size_t index;
    void *element;
    if (list == NULL || data == NULL) return false;
    if (list_is_empty(list)) return false;
    if (!list_contains(list, data)) return false;
    if (list->equals == NULL) {
        fprintf(stderr, "List: Unable to compare, please define a valid equals function\n");
        return false;
    }

    node = list->head;
    index = 0;
    while (node != NULL) {
        if (list->equals(node->data, data)) {
            element = list_remove_index(list, index);
            if (list->destroy == NULL) {
                free(element);
            } else {
                list->destroy(element);
            }
            index--;
        }
        node = node->next;
        index++;
    }

    return true;
}

bool list_contains(const List *list, const void *data) {
    Node *node;
    if (list == NULL || data == NULL) return false;
    if (list_is_empty(list)) return false;
    if (list->equals == NULL) {
        fprintf(stderr, "List: Unable to compare, please define a valid equals function\n");
        return false;
    }

    node = list->head;
    while (node != NULL && !list->equals(node->data, data)) {
        node = node->next;
    }

    return node != NULL;
}