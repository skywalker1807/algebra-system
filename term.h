#ifndef TERM_H_
#define TERM_H_

typedef enum { false, true } bool;

typedef struct Term Term;
typedef struct Literal Literal;
typedef struct Constant Constant;
typedef struct Variable Variable;
typedef struct Operator Operator;

struct Term {
    void *content;
    char *meaning;
};

struct Literal {
    double value;
};

struct Constant {
    char *name;
    double upper_limit;
    double lower_limit;
};

struct Variable {
    char *name;
    int indec;
    Term **index;
};

struct Operator {
    char *name;
    int argc;
    Term **argv;
};


char* copy_string(char* str);

Term *term(void *content, char* meaning);
Term *literal(double value);
Term *constant(char *name, double upper_limit, double lower_limit);
Term *variable(char *name);
Term *variable_with_index(char *name, int indec, Term **index);
Term *operator(char *name, int argc, Term **argv);

Literal *construct_literal(double value);
Constant *construct_constant(char *name, double upper_limit, double lower_limit);
Variable *construct_variable(char *name, int indec, Term **index);
Operator *construct_operator(char *name, int argc, Term **argv);

void free_term(Term *term);
void free_variable(Variable *variable);
void free_literal(Literal *literal);
void free_constant(Constant *contant);
void free_operator(Operator *operator);

Term *copy_term(Term *term);
Literal *copy_literal(Literal *literal);
Constant *copy_constant(Constant *constant);
Variable *copy_variable(Variable *variable);
Operator *copy_operator(Operator *operator);

void print_term(Term *term);
void print_literal(Literal *literal);
void print_constant(Constant *constant);
void print_variable(Variable *variable);
void print_operator(Operator *operator);

void print_imaginary(Operator *operator);
void print_addition(Operator *operator);
void print_additive_inverse(Operator *operator);
void print_multiply(Operator *operator);
void print_multiple_inverse(Operator *operator);
void print_differential(Operator *operator);
void print_integral(Operator *operator);
void print_equals(Operator *operator);

Operator *is_operator(Term *term, char *name);

Term *imaginary(Term *term);
Term *add(Term *lhs, Term *rhs);
Term *additive_inverse(Term *term);
Term *multiply(Term *lhs, Term *rhs);
Term *multiple_inverse(Term *term);
Term *differential(Term *term, Term* variable);
Term *integral(Term *term, Term *variable);
Term *definite_integral(Term *term, Term *variable, Term *upper_limit, Term *lower_limit);
Term *equal(Term *lhs, Term *rhs);

#endif // TERM_H_
