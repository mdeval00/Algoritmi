#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

typedef struct Term {
    int coefficient;
    int exponent;
    struct Term* next;
} Term;

typedef struct Polynomial {
    Term* head;
} Polynomial;

Term* create_term(int coeff, int exp) {
    Term* new_term = (Term*)malloc(sizeof(Term));
    if (new_term == NULL) {
        printf("Greska pri alokaciji memorije!\n");
        exit(1);
    }
    new_term->coefficient = coeff;
    new_term->exponent = exp;
    new_term->next = NULL;
    return new_term;
}

void add_term(Polynomial* polynomial, int coeff, int exp) {
    if (coeff == 0) {
        return;
    }

    Term* new_term = create_term(coeff, exp);

    if (polynomial->head == NULL || polynomial->head->exponent < exp) {
        new_term->next = polynomial->head;
        polynomial->head = new_term;
        return;
    }

    Term* current = polynomial->head;
    Term* previous = NULL;

    while (current != NULL && current->exponent > exp) {
        previous = current;
        current = current->next;
    }

    if (current != NULL && current->exponent == exp) {
        current->coefficient += coeff;
        free(new_term);

        if (current->coefficient == 0) {
            if (previous == NULL) {
                polynomial->head = current->next;
            }
            else {
                previous->next = current->next;
            }
            free(current);
        }
        return;
    }

    new_term->next = current;
    if (previous == NULL) {
        polynomial->head = new_term;
    }
    else {
        previous->next = new_term;
    }
}

Polynomial* read_polynomial(FILE* file) {
    Polynomial* polynomial = (Polynomial*)malloc(sizeof(Polynomial));
    if (polynomial == NULL) {
        printf("Greska pri alokaciji memorije!\n");
        exit(1);
    }
    polynomial->head = NULL;

    int coeff, exp;
    char buffer[100];

    while (1) {
       
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            break;
        }

       
        if (buffer[0] == '\n' || (buffer[0] == '\r' && buffer[1] == '\n')) {
            break;
        }

      
        char* ptr = buffer;
        int items_read;
        while (sscanf(ptr, "%d %d%n", &coeff, &exp, &items_read) == 2) {
            add_term(polynomial, coeff, exp);
            ptr += items_read;
        }

       
        if (ptr == buffer) {
            break;
        }
    }

    return polynomial;
}
Polynomial* add_polynomials(Polynomial* p1, Polynomial* p2) {
    Polynomial* result = (Polynomial*)malloc(sizeof(Polynomial));
    if (result == NULL) {
        printf("Greska pri alokaciji memorije!\n");
        exit(1);
    }
    result->head = NULL;

    Term* current = p1->head;
    while (current != NULL) {
        add_term(result, current->coefficient, current->exponent);
        current = current->next;
    }

    current = p2->head;
    while (current != NULL) {
        add_term(result, current->coefficient, current->exponent);
        current = current->next;
    }

    return result;
}

Polynomial* multiply_polynomials(Polynomial* p1, Polynomial* p2) {
    Polynomial* result = (Polynomial*)malloc(sizeof(Polynomial));
    if (result == NULL) {
        printf("Greska pri alokaciji memorije!\n");
        exit(1);
    }
    result->head = NULL;

    Term* i = p1->head;
    Term* j;

    while (i != NULL) {
        j = p2->head;
        while (j != NULL) {
            int new_coeff = i->coefficient * j->coefficient;
            int new_exp = i->exponent + j->exponent;
            add_term(result, new_coeff, new_exp);
            j = j->next;
        }
        i = i->next;
    }

    return result;
}

void print_polynomial(Polynomial* polynomial) {
    Term* current = polynomial->head;

    if (current == NULL) {
        printf("0");
        return;
    }

    int first = 1;
    while (current != NULL) {
        if (!first) {
            if (current->coefficient > 0) {
                printf(" + ");
            }
            else {
                printf(" - ");
            }
        }
        else if (current->coefficient < 0) {
            printf("-");
        }

        int abs_coeff = abs(current->coefficient);

        if (abs_coeff != 1 || current->exponent == 0) {
            printf("%d", abs_coeff);
        }

        if (current->exponent > 0) {
            printf("x");
            if (current->exponent > 1) {
                printf("^%d", current->exponent);
            }
        }

        first = 0;
        current = current->next;
    }
    printf("\n");
}

void free_polynomial(Polynomial* polynomial) {
    Term* current = polynomial->head;
    Term* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(polynomial);
}

int main() {
    FILE* file;
    char filename[100];

    printf("Unesite ime datoteke: ");
    scanf("%s", filename);

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Greska pri otvaranju datoteke!\n");
        return 1;
    }

    Polynomial* poly1 = read_polynomial(file);
    Polynomial* poly2 = read_polynomial(file);

    fclose(file);

    printf("\nPrvi polinom: ");
    print_polynomial(poly1);

    printf("Drugi polinom: ");
    print_polynomial(poly2);

    Polynomial* sum = add_polynomials(poly1, poly2);
    printf("Zbroj polinoma: ");
    print_polynomial(sum);

    Polynomial* product = multiply_polynomials(poly1, poly2);
    printf("Umnozak polinoma: ");
    print_polynomial(product);

    free_polynomial(poly1);
    free_polynomial(poly2);
    free_polynomial(sum);
    free_polynomial(product);

    return 0;
}