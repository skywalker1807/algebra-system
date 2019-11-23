#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "term.h"


char*
copy_string(char* str)
{
    char *value = (char*) malloc(sizeof(char) * (strlen(str) + 1));

    strcpy(value, str);

    return value;
}

Term *
term(void *content, char* meaning)
{
    Term *term = (Term *) malloc(sizeof(Term));

    term->content = content;
    term->meaning = copy_string(meaning);

    return term;
}

Term *
literal(double value)
{
    Literal *literal = construct_literal(value);

    return term(literal, "literal");
}

Term *
constant(char *name, double upper_limit, double lower_limit)
{
    Constant *constant = construct_constant(name, upper_limit, lower_limit);

    return term(constant, "constant");
}

Term *
variable(char *name)
{
    Variable *variable = construct_variable(name, 0, NULL);

    return term(variable, "variable");
}

Term *
variable_with_index(char *name, int indec, Term **index)
{
    Variable *variable = construct_variable(name, indec, index);

    return term(variable, "variable");
}

Term *
operator(char *name, int argc, Term **argv)
{
    Operator *operator = construct_operator(name, argc, argv);

    return term(operator, "operator");
}

Literal *
construct_literal(double value)
{
    Literal* literal = (Literal *) malloc(sizeof(Literal));

    literal->value = value;

    return literal;
}

Constant *
construct_constant(char *name, double upper_limit, double lower_limit)
{
    Constant *constant = (Constant *) malloc(sizeof(Constant));

    constant->name = copy_string(name);
    constant->upper_limit = upper_limit;
    constant->lower_limit = lower_limit;

    return constant;
}

Variable *
construct_variable(char *name, int indec, Term **index)
{
    Variable* variable = (Variable *) malloc(sizeof(Variable));

    variable->name = copy_string(name);
    variable->indec = indec;
    variable->index = index;

    return variable;
}

Operator *
construct_operator(char *name, int argc, Term **argv)
{
    Operator *operator = (Operator *) malloc(sizeof(Operator));

    operator->name = copy_string(name);
    operator->argc = argc;
    operator->argv = argv;

    return operator;
}

void
free_term(Term *term)
{
    if (strcmp(term->meaning, "literal") == 0)
        free_literal(term->content);
    if (strcmp(term->meaning, "constant") == 0)
        free_constant(term->content);
    if (strcmp(term->meaning, "variable") == 0)
        free_variable(term->content);
    if (strcmp(term->meaning, "operator") == 0)
        free_operator(term->content);
    free(term->meaning);
    free(term);
    return;
}

void
free_literal(Literal *literal)
{
    free(literal);
    return;
}

void
free_constant(Constant *constant)
{
    free(constant->name);
    free(constant);
    return;
}

void
free_variable(Variable *variable)
{
    for (int i = 0;i < variable->indec;i++)
        free_term(variable->index[i]);
    free(variable->index);
    free(variable->name);
    free(variable);
    return;
}

void
free_operator(Operator *operator)
{
    for (int i = 0;i < operator->argc;i++)
        free_term(operator->argv[i]);
    free(operator->argv);
    free(operator->name);
    free(operator);
    return;
}

Term *
copy_term(Term *term)
{
    Term *new = (Term *) malloc(sizeof(Term));

    new->meaning = copy_string(term->meaning);

    if(strcmp(term->meaning, "literal") == 0)
        new->content = copy_literal(term->content);
    if(strcmp(term->meaning, "constant") == 0)
        new->content = copy_constant(term->content);
    if(strcmp(term->meaning, "variable") == 0)
        new->content = copy_variable(term->content);
    if(strcmp(term->meaning, "operator") == 0)
        new->content = copy_operator(term->content);

    return new;
}

Literal *
copy_literal(Literal *literal)
{
    return construct_literal(literal->value);
}

Constant *
copy_constant(Constant *constant)
{
    return construct_constant(constant->name, constant->upper_limit, constant->lower_limit);
}

Variable *
copy_variable(Variable *variable)
{
    Term **index = (Term **) malloc(sizeof(Term *) * variable->indec);

    for(int i = 0;i < variable->indec;i++)
        index[i] = copy_term(variable->index[i]);

    return construct_variable(variable->name, variable->indec, index);
}

Operator *
copy_operator(Operator *operator)
{
    Term **argv = (Term **) malloc(sizeof(Term *) * operator->argc);

    for(int i = 0;i < operator->argc;i++)
        argv[i] = copy_term(operator->argv[i]);

    return construct_operator(operator->name, operator->argc, argv);
}

void
print_term(Term *term)
{
    if (strcmp(term->meaning, "literal") == 0)
        print_literal(term->content);
    if (strcmp(term->meaning, "constant") == 0)
        print_constant(term->content);
    if (strcmp(term->meaning, "variable") == 0)
        print_variable(term->content);
    if (strcmp(term->meaning, "operator") == 0)
        print_operator(term->content);
    return;
}

void
print_literal(Literal *literal)
{
    printf("%.6g", literal->value);
    return;
}

void
print_constant(Constant *constant)
{
    printf("%s", constant->name);
    return;
}

void
print_variable(Variable *variable)
{
    printf("%s", variable->name);

    if (variable->indec != 0) {
        printf("_{");
        for (int i = 0;i < variable->indec;i++) {
            print_term(variable->index[i]);
            if (i != variable->indec - 1)
                printf(", ");
        }
        printf("}");
    }
    return;
}

void
print_operator(Operator *operator)
{
    if (strcmp(operator->name, "imaginary") == 0)
        print_imaginary(operator);
    if (strcmp(operator->name, "+") == 0)
        print_addition(operator);
    if (strcmp(operator->name, "additive_inverse") == 0)
        print_additive_inverse(operator);
    if (strcmp(operator->name, "*") == 0)
        print_multiply(operator);
    if (strcmp(operator->name, "multiple_inverse") == 0)
        print_multiple_inverse(operator);
    if (strcmp(operator->name, "D") == 0)
        print_differential(operator);
    if (strcmp(operator->name, "I") == 0)
        print_integral(operator);
    if (strcmp(operator->name, "=") == 0)
        print_equals(operator);
    return;
}

void
print_imaginary(Operator *operator)
{
    printf("i*");
    print_term(operator->argv[0]);
    return;
}

void
print_addition(Operator *operator)
{
    printf("(");

    print_term(operator->argv[0]);
    printf(" + ");
    print_term(operator->argv[1]);

    for(int i = 2;i < operator->argc;i++) {
        printf(" + ");
        print_term(operator->argv[i]);
    }
    printf(")");
    return;
}

void
print_additive_inverse(Operator *operator)
{
    printf("(-");
    print_term(operator->argv[0]);
    printf(")");
    return;
}

void
print_multiply(Operator *operator)
{
    printf("(");

    print_term(operator->argv[0]);
    printf(" * ");
    print_term(operator->argv[1]);

    for(int i = 2;i < operator->argc;i++) {
        printf(" * ");
        print_term(operator->argv[i]);
    }
    printf(")");
    return;
}

void
print_multiple_inverse(Operator *operator)
{
    printf("(1.0/");
    print_term(operator->argv[0]);
    printf(")");
    return;
}

void
print_differential(Operator *operator)
{
    printf("D[");
    print_term(operator->argv[0]);
    printf(", ");
    print_term(operator->argv[1]);
    printf("]");
    return;
}

void
print_integral(Operator *operator)
{
    printf("I[");
    print_term(operator->argv[0]);
    printf(", ");
    print_term(operator->argv[1]);

    if (operator->argc) {
        printf(", ");
        print_term(operator->argv[2]);
        printf(", ");
        print_term(operator->argv[3]);
    }
    printf("]");
    return;
}

void
print_equals(Operator *operator)
{
    print_term(operator->argv[0]);
    printf(" = ");
    print_term(operator->argv[1]);
    return;
}

Operator *is_operator(Term *term, char *name)
{
    Operator *operator;

    if (strcmp(term->meaning, "operator") != 0)
        return NULL;

    operator = term->content;

    if (strcmp(operator->name, name) != 0)
        return NULL;

    return operator;
}

Term *
imaginary(Term *term)
{
    Term **argv = (Term **) malloc(sizeof(Term *));

    argv[0] = term;

    return operator("imaginary", 1, argv);
}

Term *
add(Term *lhs, Term *rhs)
{
    Operator *lhs_operator, *rhs_operator;
    Term **argv;
    int argc, i;

    argc = 0;
    lhs_operator = is_operator(lhs, "+");
    if (lhs_operator == NULL)
        argc += 1;
    else
        argc += lhs_operator->argc;

    rhs_operator = is_operator(rhs, "+");
    if (rhs_operator == NULL)
        argc += 1;
    else
        argc += rhs_operator->argc;

    i = 0;
    argv = (Term **) malloc(sizeof(Term *) * argc);

    if(lhs_operator == NULL) {
        argv[i] = lhs;
        i++;
    } else {
        for(int j = 0;j < lhs_operator->argc;j++, i++)
            argv[i] = lhs_operator->argv[j]; 
    }

    if(rhs_operator == NULL) {
        argv[i] = rhs;
        i++;
    } else {
        for(int j = 0;j < rhs_operator->argc;j++, i++)
            argv[i] = rhs_operator->argv[j];
    }

    return operator("+", argc, argv);
}

Term *
additive_inverse(Term *term)
{
    Term **argv = (Term **) malloc(sizeof(Term *));

    argv[0] = term;

    return operator("additive_inverse", 1, argv);
}

Term *
multiply(Term *lhs, Term *rhs)
{
    Operator *lhs_operator, *rhs_operator;
    Term **argv;
    int argc, i;

    argc = 0;
    lhs_operator = is_operator(lhs, "*");
    if (lhs_operator == NULL)
        argc += 1;
    else
        argc += lhs_operator->argc;

    rhs_operator = is_operator(rhs, "*");
    if (rhs_operator == NULL)
        argc += 1;
    else
        argc += rhs_operator->argc;

    i = 0;
    argv = (Term **) malloc(sizeof(Term *) * argc);

    if(lhs_operator == NULL) {
        argv[i] = lhs;
        i++;
    } else {
        for(int j = 0;j < lhs_operator->argc;j++, i++)
            argv[i] = lhs_operator->argv[j]; 
    }

    if(rhs_operator == NULL) {
        argv[i] = rhs;
        i++;
    } else {
        for(int j = 0;j < rhs_operator->argc;j++, i++)
            argv[i] = rhs_operator->argv[j];
    }

    return operator("*", argc, argv);
}

Term *
multiple_inverse(Term *term)
{
    Term **argv = (Term **) malloc(sizeof(Term *));

    argv[0] = term;

    return operator("multiple_inverse", 1, argv);
}

Term *
differential(Term *term, Term* variable)
{
    Term **argv = (Term **) malloc(sizeof(Term *) + 2);

    argv[0] = term;
    argv[1] = variable;

    return operator("D", 2, argv);
}

Term *
integral(Term *term, Term *variable)
{
    Term **argv = (Term **) malloc(sizeof(Term *) + 2);

    argv[0] = term;
    argv[1] = variable;

    return operator("I", 2, argv);
}

Term *
definite_integral(Term *term, Term *variable, Term *upper_limit, Term *lower_limit)
{
    Term **argv = (Term **) malloc(sizeof(Term *) + 4);

    argv[0] = term;
    argv[1] = variable;
    argv[3] = upper_limit;
    argv[4] = lower_limit;

    return operator("I", 4, argv);
}

Term *
equal(Term *lhs, Term *rhs)
{
    Term **argv = (Term **) malloc(sizeof(Term *) * 2);

    argv[0] = lhs;
    argv[1] = rhs;

    return operator("=", 2, argv);
}
