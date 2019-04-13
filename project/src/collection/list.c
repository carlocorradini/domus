
#include <stdlib.h>
#include <stdio.h>
#include "data_structure/list.h"

List *list_create(void *data) {
    Node *new_node;

    List *new_list = (List *) malloc(sizeof(List));
    if (new_list == NULL) {
        perror("List Memory Allocation");
        exit(EXIT_FAILURE);
    }
    *new_list = (Node *) malloc(sizeof(Node));

    new_node = *new_list;
    new_node->data = data;
    new_node->next = NULL;
    return new_list;
}

void list_free(List *list) {
    Node *curr = *list;
    Node *next;

    while (curr != NULL) {
        next = curr->next;
        free(curr);
        curr = next;
    }

    free(list);
}

void list_push(List *list, void *data) {
    Node *head;
    Node *new_node;

    if (list == NULL || *list == NULL) {
        fprintf(stderr, "list_push: List has not been initialized\n");
        return;
    }

    head = *list;

    if (head->data == NULL) {
        /* Head is an empty node */
        head->data = data;
    } else {
        /* Head is not empty, add new node as new head */
        new_node = malloc(sizeof(Node));
        if (new_node == NULL) {
            perror("List Memory Allocation");
            exit(EXIT_FAILURE);
        }
        new_node->data = data;
        new_node->next = head;
        *list = new_node;
    }
}

void *list_pop(List *list) {
    void *data;
    Node *head = *list;

    if (head == NULL || head->data == NULL) {
        return NULL;
    }

    data = head->data;
    *list = head->next;

    free(head);
    return data;
}