#ifndef VARIABLE_TERM_H_
#define VARIABLE_TERM_H_

bool is_variable_term(Term* term);
Term *get_variable_term(Term* term);
Term *get_non_variable_term(Term* term);

#endif /* VARIABLE_TERM_H_ */
