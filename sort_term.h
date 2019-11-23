#ifndef SORT_TERM_H_
#define SORT_TERM_H_

void merge_sort_terms(Term *arr[], int l, int r);
void merge_terms(Term *arr[], int l, int m, int r);

void merge_sort_variable_terms(Term *arr[], int l, int r);
void merge_variable_terms(Term *arr[], int l, int m, int r);

#endif // SORT_TERM_H_
