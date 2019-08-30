#ifndef SORT_TERMS_H
#define SORT_TERMS_H

#include "compare_terms.h"
#include "variable_terms.h"

static void merge_sort_multiplied_terms(Term *arr[], int l, int r);
static void merge_sort_added_terms(Term *arr[], int l, int r);
static void merge_multiplied_terms(Term *arr[], int l, int m, int r);
static void merge_added_terms(Term *arr[], int l, int m, int r);

void
merge_sort_multiplied_terms(Term *arr[], int l, int r)
{
    if (l < r) {
        int m = l + (r - l)/2;

        merge_sort_multiplied_terms(arr, l, m);
        merge_sort_multiplied_terms(arr, m + 1, r);

        merge_multiplied_terms(arr, l, m, r);
    }
}

void
merge_sort_added_terms(Term *arr[], int l, int r)
{
    if (l < r) {
        int m = l + (r - l)/2;

        merge_sort_added_terms(arr, l, m);
        merge_sort_added_terms(arr, m + 1, r);

        merge_added_terms(arr, l, m, r);
    }
}

void
merge_multiplied_terms(Term *arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    Term *L[n1], *R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (is_less(L[i], R[j]) || is_equal(L[i], R[j])) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void
merge_added_terms(Term *arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    Term *L[n1], *R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (is_greater(
                get_variable_terms_from_multiple(L[i]),
                get_variable_terms_from_multiple(R[j])) ||
           is_equal(
                get_variable_terms_from_multiple(L[i]),
                get_variable_terms_from_multiple(R[j]))) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

#endif /* SORT_TERMS_H */
