#ifndef SORT_TERM_H_
#define SORT_TERM_H_

void merge_sort_terms(Term *array[], int left, int right);
void merge_terms(Term *array[], int left, int middle, int right);

void merge_sort_variable_terms(Term *array[], int left, int right);
void merge_variable_terms(Term *array[], int left, int middle, int right);

#endif // SORT_TERM_H_
