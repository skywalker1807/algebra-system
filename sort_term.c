#include "term.h"
#include "compare_term.h"
#include "variable_term.h"
#include "sort_term.h"

void
merge_sort_terms(Term *array[], int left, int right)
{
    if (left < right) {
        int middle = (right + left) >> 1;

        merge_sort_terms(array, left, middle);
        merge_sort_terms(array, middle + 1, right);

        merge_terms(array, left, middle, right);
    }
}

void
merge_terms(Term *array[], int left, int middle, int right)
{
    int i, j, k;
    int n1 = middle - left + 1;
    int n2 = right - middle;

    Term *L[n1], *R[n2];

    for (i = 0; i < n1; i++)
        L[i] = array[left + i];
    for (j = 0; j < n2; j++)
        R[j] = array[middle + 1 + j];

    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (is_less(L[i], R[j]) || is_equal(L[i], R[j])) {
            array[k] = L[i];
            i++;
        } else {
            array[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        array[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        array[k] = R[j];
        j++;
        k++;
    }
}

void  
merge_sort_variable_terms(Term *array[], int left, int right)
{
    if (left < right) {
        int middle = (right + left) >> 1;

        merge_sort_variable_terms(array, left, middle);
        merge_sort_variable_terms(array, middle + 1, right);

        merge_variable_terms(array, left, middle, right);
    }
}

void
merge_variable_terms(Term *array[], int left, int middle, int right)
{
    int i, j, k;
    int n1 = middle - left + 1;
    int n2 = right - middle;

    Term *L[n1], *R[n2];

    for (i = 0; i < n1; i++)
        L[i] = array[left + i];
    for (j = 0; j < n2; j++)
        R[j] = array[middle + 1 + j];

    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (is_greater(
                get_variable_term(L[i]),
                get_variable_term(R[j])) ||
           is_equal(
                get_variable_term(L[i]),
                get_variable_term(R[j]))) {
            array[k] = L[i];
            i++;
        } else {
            array[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        array[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        array[k] = R[j];
        j++;
        k++;
    }
}
