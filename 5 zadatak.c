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

void free_list(List* list);

Node* create_node(int data, int* error) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        *error = 1;
        return NULL;
    }
    new_node->data = data;
    new_node->next = NULL;
    *error = 0;
    return new_node;
}

void insert_sorted(List* list, int data, int* error) {
    Node* new_node = create_node(data, error);
    if (*error) return;

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

List* union_lists(List* L1, List* L2, int* error) {
    List* result = (List*)malloc(sizeof(List));
    if (result == NULL) {
        *error = 1;
        return NULL;
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

        new_node = create_node(data_to_add, error);
        if (*error) {
            free_list(result);
            return NULL;
        }

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
        Node* new_node = create_node(current1->data, error);
        if (*error) {
            free_list(result);
            return NULL;
        }
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
        Node* new_node = create_node(current2->data, error);
        if (*error) {
            free_list(result);
            return NULL;
        }
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

List* intersection_lists(List* L1, List* L2, int* error) {
    List* result = (List*)malloc(sizeof(List));
    if (result == NULL) {
        *error = 1;
        return NULL;
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
            Node* new_node = create_node(current1->data, error);
            if (*error) {
                free_list(result);
                return NULL;
            }
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
    if (list == NULL) return;

    Node* current = list->head;
    Node* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(list);
}

void read_list_from_console(List* list, const char* list_name, int* error) {
    printf("Unesite broj elemenata za listu %s: ", list_name);
    int n;
    if (scanf("%d", &n) != 1 || n < 0) {
        *error = 1;
        return;
    }

    printf("Unesite %d sortiranih brojeva za listu %s:\n", n, list_name);
    for (int i = 0; i < n; i++) {
        int value;
        if (scanf("%d", &value) != 1) {
            *error = 1;
            return;
        }
        insert_sorted(list, value, error);
        if (*error) return;
    }
}

void read_list_from_file(List* list, const char* filename, int* error) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        *error = 1;
        return;
    }

    int value;
    while (fscanf(file, "%d", &value) == 1) {
        insert_sorted(list, value, error);
        if (*error) {
            fclose(file);
            return;
        }
    }

    fclose(file);
}

int main() {
    int error = 0;

    List* L1 = (List*)malloc(sizeof(List));
    List* L2 = (List*)malloc(sizeof(List));

    if (L1 == NULL || L2 == NULL) {
        printf("Greska pri alokaciji memorije!\n");
        if (L1) free(L1);
        if (L2) free(L2);
        return 1;
    }

    L1->head = NULL;
    L2->head = NULL;

    int choice;
    printf("Odaberite nacin unosa listi:\n");
    printf("1. Unos iz konzole\n");
    printf("2. Citanje iz datoteka\n");
 
    if (scanf("%d", &choice) != 1) {
        printf("Greska pri unosu!\n");
        free(L1);
        free(L2);
        return 1;
    }

    if (choice == 1) {
        read_list_from_console(L1, "L1", &error);
        if (error) {
            printf("Greska pri unosu liste L1!\n");
            free_list(L1);
            free_list(L2);
            return 1;
        }

        read_list_from_console(L2, "L2", &error);
        if (error) {
            printf("Greska pri unosu liste L2!\n");
            free_list(L1);
            free_list(L2);
            return 1;
        }
    }
    else if (choice == 2) {
        char filename1[100], filename2[100];
        printf("Unesite ime datoteke za listu L1: ");
        scanf("%s", filename1);
        printf("Unesite ime datoteke za listu L2: ");
        scanf("%s", filename2);

        read_list_from_file(L1, filename1, &error);
        if (error) {
            printf("Greska pri citanju datoteke %s!\n", filename1);
            free_list(L1);
            free_list(L2);
            return 1;
        }

        read_list_from_file(L2, filename2, &error);
        if (error) {
            printf("Greska pri citanju datoteke %s!\n", filename2);
            free_list(L1);
            free_list(L2);
            return 1;
        }
    }
    else {
        printf("Nevazeci izbor!\n");
        free(L1);
        free(L2);
        return 1;
    }

    printf("\nPrva lista (L1): ");
    print_list(L1);
    printf("\n");

    printf("Druga lista (L2): ");
    print_list(L2);
    printf("\n");

    List* union_result = union_lists(L1, L2, &error);
    if (error) {
        printf("Greska pri izracunavanju unije!\n");
        free_list(L1);
        free_list(L2);
        return 1;
    }
    printf("\nUnija L1 U L2: ");
    print_list(union_result);
    printf("\n");

    List* intersection_result = intersection_lists(L1, L2, &error);
    if (error) {
        printf("Greska pri izracunavanju presjeka!\n");
        free_list(L1);
        free_list(L2);
        free_list(union_result);
        return 1;
    }
    printf("Presjek L1 // L2: ");
    print_list(intersection_result);
    printf("\n");

    free_list(L1);
    free_list(L2);
    free_list(union_result);
    free_list(intersection_result);

    return 0;
}
