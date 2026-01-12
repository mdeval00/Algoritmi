#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct List {
    Node* head;
} List;

Node* create_node(int data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

void insert_sorted(List* list, int data) {
    Node* new_node = create_node(data);

    if (list->head == NULL || list->head->data >= data) {
        if (list->head != NULL && list->head->data == data) {
            free(new_node);
            return;
        }
        new_node->next = list->head;
        list->head = new_node;
        return;
    }

    Node* current = list->head;
    while (current->next != NULL && current->next->data < data) {
        current = current->next;
    }

    if (current->next != NULL && current->next->data == data) {
        free(new_node);
        return;
    }

    new_node->next = current->next;
    current->next = new_node;
}

List* union_lists(List* L1, List* L2) {
    List* result = (List*)malloc(sizeof(List));
    if (result == NULL) {
        printf("Greska pri alokaciji memorije!\n");
        exit(1);
    }
    result->head = NULL;

    Node* current1 = L1->head;
    Node* current2 = L2->head;
    Node* tail = NULL;

    while (current1 != NULL && current2 != NULL) {
        Node* new_node;
        int data_to_add;

        if (current1->data < current2->data) {
            data_to_add = current1->data;
            current1 = current1->next;
        }
        else if (current2->data < current1->data) {
            data_to_add = current2->data;
            current2 = current2->next;
        }
        else {
            data_to_add = current1->data;
            current1 = current1->next;
            current2 = current2->next;
        }

        new_node = create_node(data_to_add);

        if (result->head == NULL) {
            result->head = new_node;
            tail = new_node;
        }
        else {
            tail->next = new_node;
            tail = new_node;
        }
    }

    while (current1 != NULL) {
        Node* new_node = create_node(current1->data);
        if (result->head == NULL) {
            result->head = new_node;
            tail = new_node;
        }
        else {
            tail->next = new_node;
            tail = new_node;
        }
        current1 = current1->next;
    }

    while (current2 != NULL) {
        Node* new_node = create_node(current2->data);
        if (result->head == NULL) {
            result->head = new_node;
            tail = new_node;
        }
        else {
            tail->next = new_node;
            tail = new_node;
        }
        current2 = current2->next;
    }

    return result;
}

List* intersection_lists(List* L1, List* L2) {
    List* result = (List*)malloc(sizeof(List));
    if (result == NULL) {
        printf("Greska pri alokaciji memorije!\n");
        exit(1);
    }
    result->head = NULL;

    Node* current1 = L1->head;
    Node* current2 = L2->head;
    Node* tail = NULL;

    while (current1 != NULL && current2 != NULL) {
        if (current1->data < current2->data) {
            current1 = current1->next;
        }
        else if (current2->data < current1->data) {
            current2 = current2->next;
        }
        else {
            Node* new_node = create_node(current1->data);
            if (result->head == NULL) {
                result->head = new_node;
                tail = new_node;
            }
            else {
                tail->next = new_node;
                tail = new_node;
            }
            current1 = current1->next;
            current2 = current2->next;
        }
    }

    return result;
}

void print_list(List* list) {
    Node* current = list->head;

    if (current == NULL) {
        printf("Lista je prazna");
        return;
    }

    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(" -> ");
        }
        current = current->next;
    }
}

void free_list(List* list) {
    Node* current = list->head;
    Node* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(list);
}

int main() {
    List* L1 = (List*)malloc(sizeof(List));
    List* L2 = (List*)malloc(sizeof(List));

    if (L1 == NULL || L2 == NULL) {
        printf("Greska pri alokaciji memorije!\n");
        return 1;
    }

    L1->head = NULL;
    L2->head = NULL;

    insert_sorted(L1, 1);
    insert_sorted(L1, 3);
    insert_sorted(L1, 5);
    insert_sorted(L1, 7);
    insert_sorted(L1, 9);

    insert_sorted(L2, 2);
    insert_sorted(L2, 3);
    insert_sorted(L2, 5);
    insert_sorted(L2, 8);
    insert_sorted(L2, 9);

    printf("Prva lista (L1): ");
    print_list(L1);
    printf("\n");

    printf("Druga lista (L2): ");
    print_list(L2);
    printf("\n");

    List* union_result = union_lists(L1, L2);
    printf("\nUnija L1 U L2: ");
    print_list(union_result);
    printf("\n");

    List* intersection_result = intersection_lists(L1, L2);
    printf("Presjek L1 // L2: ");
    print_list(intersection_result);
    printf("\n");

    free_list(L1);
    free_list(L2);
    free_list(union_result);
    free_list(intersection_result);

    return 0;
}