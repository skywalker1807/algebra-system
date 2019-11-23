#ifndef VARIABLE_TERM_H_
#define VARIABLE_TERM_H_

bool is_variable_term(Term* t);
Term *get_variable_term(Term* t);
Term *get_non_variable_term(Term* t);

#endif /* VARIABLE_TERM_H_ */
