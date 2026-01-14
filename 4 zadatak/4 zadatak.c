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

void free_polynomial(Polynomial* polynomial);

Term* create_term(int coeff, int exp, int* error) {
    Term* new_term = (Term*)malloc(sizeof(Term));
    if (new_term == NULL) {
        *error = 1;
        return NULL;
    }
    new_term->coefficient = coeff;
    new_term->exponent = exp;
    new_term->next = NULL;
    return new_term;
}

int add_term(Polynomial* polynomial, int coeff, int exp) {
    if (coeff == 0) {
        return 0;
    }

    int error = 0;
    Term* new_term = create_term(coeff, exp, &error);
    if (error) {
        return 1;
    }

    if (polynomial->head == NULL || polynomial->head->exponent < exp) {
        new_term->next = polynomial->head;
        polynomial->head = new_term;
        return 0;
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
        return 0;
    }

    new_term->next = current;
    if (previous == NULL) {
        polynomial->head = new_term;
    }
    else {
        previous->next = new_term;
    }

    return 0;
}

int read_polynomials_from_file(const char* filename, Polynomial** poly1, Polynomial** poly2) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Greska pri otvaranju datoteke '%s'!\n", filename);
        return 1;
    }

    *poly1 = (Polynomial*)malloc(sizeof(Polynomial));
    *poly2 = (Polynomial*)malloc(sizeof(Polynomial));

    if (*poly1 == NULL || *poly2 == NULL) {
        printf("Greska pri alokaciji memorije!\n");
        fclose(file);
        free(*poly1);
        free(*poly2);
        *poly1 = NULL;
        *poly2 = NULL;
        return 1;
    }

    (*poly1)->head = NULL;
    (*poly2)->head = NULL;

    int coeff, exp;
    int reading_first = 1;
    int error = 0;

    while (fscanf(file, "%d %d", &coeff, &exp) == 2) {
        if (reading_first) {
            error = add_term(*poly1, coeff, exp);
        }
        else {
            error = add_term(*poly2, coeff, exp);
        }

        if (error) {
            printf("Greska pri obradi polinoma!\n");
            fclose(file);
            return 1;
        }

        char c = fgetc(file);
        if (c == '\n') {
            reading_first = 0;
        }
        else if (c != EOF) {
            ungetc(c, file);
        }
    }

    if (ferror(file)) {
        printf("Greska pri citanju datoteke!\n");
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}

Polynomial* add_polynomials(Polynomial* p1, Polynomial* p2, int* error) {
    Polynomial* result = (Polynomial*)malloc(sizeof(Polynomial));
    if (result == NULL) {
        *error = 1;
        return NULL;
    }
    result->head = NULL;

    Term* current = p1->head;
    while (current != NULL) {
        if (add_term(result, current->coefficient, current->exponent)) {
            *error = 1;
            free_polynomial(result);
            return NULL;
        }
        current = current->next;
    }

    current = p2->head;
    while (current != NULL) {
        if (add_term(result, current->coefficient, current->exponent)) {
            *error = 1;
            free_polynomial(result);
            return NULL;
        }
        current = current->next;
    }

    *error = 0;
    return result;
}

Polynomial* multiply_polynomials(Polynomial* p1, Polynomial* p2, int* error) {
    Polynomial* result = (Polynomial*)malloc(sizeof(Polynomial));
    if (result == NULL) {
        *error = 1;
        return NULL;
    }
    result->head = NULL;

    Term* i = p1->head;
    Term* j;

    while (i != NULL) {
        j = p2->head;
        while (j != NULL) {
            int new_coeff = i->coefficient * j->coefficient;
            int new_exp = i->exponent + j->exponent;
            if (add_term(result, new_coeff, new_exp)) {
                *error = 1;
                free_polynomial(result);
                return NULL;
            }
            j = j->next;
        }
        i = i->next;
    }

    *error = 0;
    return result;
}

void print_polynomial(Polynomial* polynomial) {
    if (polynomial == NULL) {
        printf("(NULL)\n");
        return;
    }

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
    if (polynomial == NULL) {
        return;
    }

    Term* current = polynomial->head;
    Term* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(polynomial);
}

void cleanup(Polynomial* p1, Polynomial* p2, Polynomial* sum, Polynomial* product) {
    free_polynomial(p1);
    free_polynomial(p2);
    free_polynomial(sum);
    free_polynomial(product);
}

int main() {
    char filename[100];
    Polynomial* poly1 = NULL;
    Polynomial* poly2 = NULL;
    Polynomial* sum = NULL;
    Polynomial* product = NULL;
    int error = 0;

    printf("Unesite ime datoteke: ");
    scanf("%s", filename);

    if (read_polynomials_from_file(filename, &poly1, &poly2)) {
        cleanup(poly1, poly2, sum, product);
        return 1;
    }

    printf("\nPrvi polinom: ");
    print_polynomial(poly1);

    printf("Drugi polinom: ");
    print_polynomial(poly2);

    sum = add_polynomials(poly1, poly2, &error);
    if (error) {
        printf("Greska pri zbrajanju polinoma!\n");
        cleanup(poly1, poly2, sum, product);
        return 1;
    }

    printf("Zbroj polinoma: ");
    print_polynomial(sum);

    product = multiply_polynomials(poly1, poly2, &error);
    if (error) {
        printf("Greska pri mnozenju polinoma!\n");
        cleanup(poly1, poly2, sum, product);
        return 1;
    }

    printf("Umnozak polinoma: ");
    print_polynomial(product);

    cleanup(poly1, poly2, sum, product);
    return 0;
}
