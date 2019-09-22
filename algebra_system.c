#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { false, true } bool;

extern int errno;

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

static char* copy_string(char* str);

static Term *term(void *content, char* meaning);
static Term *literal(double value);
static Term *constant(char *name, double upper_limit, double lower_limit);
static Term *variable(char *name);
static Term *variable_with_index(char *name, int indec, Term **index);
static Term *operator(char *name, int argc, Term **argv);

static Literal *construct_literal(double value);
static Constant *construct_constant(char *name, double upper_limit, double lower_limit);
static Variable *construct_variable(char *name, int indec, Term **index);
static Operator *construct_operator(char *name, int argc, Term **argv);

static void free_term(Term *t);
static void free_variable(Variable *v);
static void free_literal(Literal *l);
static void free_constant(Constant *c);
static void free_operator(Operator *o);

static Term *copy_term(Term *t);
static Literal *copy_literal(Literal *l);
static Constant *copy_constant(Constant *c);
static Variable *copy_variable(Variable *v);
static Operator *copy_operator(Operator *o);

static void print_term(Term *t);
static void print_variable(Variable *v);
static void print_literal(Literal *l);
static void print_constant(Constant *c);
static void print_operator(Operator *o);

static void print_imaginary(Operator *o);
static void print_additive_inverse(Operator *o);
static void print_multiple_inverse(Operator *o);
static void print_addition(Operator *o);
static void print_subtract(Operator *o);
static void print_multiply(Operator *o);
static void print_divide(Operator *o);
static void print_equals(Operator *o);

static Operator *is_operator(Term *t, char *name);

static Term *imaginary(Term *t);
static Term *additive_inverse(Term *t);
static Term *multiple_inverse(Term *t);
static Term *add(Term *lhs, Term *rhs);
static Term *subtract(Term *lhs, Term *rhs);
static Term *multiply(Term *lhs, Term *rhs);
static Term *divide(Term *lhs, Term *rhs);
static Term *equal(Term *lhs, Term *rhs);

static Term *simplify(Term *t);
static Term *simplify_double_additive_inverse(Term *t);
static Term *simplify_double_multiple_inverse(Term *t);
static Term *simplify_double_imaginary(Term *t);
static Term *simplify_additive_assoziativity(Term *t);
static Term *simplify_multiple_assoziativity(Term *t);
static Term *simplify_multiple_additive_inverse(Term *t);
static Term *simplify_multiple_imaginary(Term *t);
static Term *simplify_multiple_inverse_imaginary(Term *t);
static Term *simplify_multiple_inverse_additive_inverse(Term *t);
static Term *swap_additive_inverse_imaginary(Term *t);
static Term *simplify_additive_inverse_add(Term *t);
static Term *simplify_imaginary_add(Term *t);
static Term *simplify_order_added_terms(Term *t);
static Term *simplify_order_multiplied_terms(Term *t);
static Term *simplify_combine_multiplied_multiple_inverse(Term *t);
static Term *simplify_combine_multiplied_neutral_terms(Term *t);
static Term *simplify_combine_added_neutral_terms(Term *t);
static Term *simplify_with_distributive_law(Term *t);
static Term *simplify_recursive_multiple_inverse(Term *t);
static Term *simplify_combine_additive_variable_terms(Term *t);
static Term *simplify_imaginary_zero(Term *t);
static Term *simplify_additive_inverse_zero(Term *t);
static Term *simplify_added_zero(Term *t);
static Term *simplify_multiplied_one(Term *t);
static Term *simplify_multiplied_zero(Term *t);
static Term *simplify_multiple_inverse_one(Term *t);
static Term *add_literals(Term *t);
static Term *add_imaginary_literals(Term *t);
static Term *multiply_literals(Term *t);

static Term *simplify_partial_fraction_expansion(Term *t);

/*
static bool is_polinomial(Term *t);
static Term *split_polinomial(Term *t);
static int max_polinomial(Term *t);
static Term **get_placeholder_terms(Term *t, Term *t);
*/

static Term *has_multiple_inverse(Term *t);

#include "compare_terms.h"
#include "variable_terms.h"
#include "sort_terms.h"

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
    Term *t = (Term *) malloc(sizeof(Term));

    t->content = content;
    t->meaning = copy_string(meaning);

    return t;
}

Term *
literal(double value)
{
    Literal *l = construct_literal(value);

    return term(l, "literal");
}

Term *
constant(char *name, double upper_limit, double lower_limit)
{
    Constant *c = construct_constant(name, upper_limit, lower_limit);

    return term(c, "constant");
}

Term *
variable(char *name)
{
    Variable *v = construct_variable(name, 0, NULL);

    return term(v, "variable");
}

Term *
variable_with_index(char *name, int indec, Term **index)
{
    Variable *v = construct_variable(name, indec, index);

    return term(v, "variable");
}

Term *
operator(char *name, int argc, Term **argv)
{
    Operator *o = construct_operator(name, argc, argv);

    return term(o, "operator");
}

Literal *
construct_literal(double value)
{
    Literal* l = (Literal *) malloc(sizeof(Literal));

    l->value = value;

    return l;
}

Constant *
construct_constant(char *name, double upper_limit, double lower_limit)
{
    Constant *c = (Constant *) malloc(sizeof(Constant));

    c->name = copy_string(name);
    c->upper_limit = upper_limit;
    c->lower_limit = lower_limit;

    return c;
}

Variable *
construct_variable(char *name, int indec, Term **index)
{
    Variable* v = (Variable *) malloc(sizeof(Variable));

    v->name = copy_string(name);
    v->indec = indec;
    v->index = index;

    return v;
}

Operator *
construct_operator(char *name, int argc, Term **argv)
{
    Operator *o = (Operator *) malloc(sizeof(Operator));

    o->name = copy_string(name);
    o->argc = argc;
    o->argv = argv;

    return o;
}

void
free_term(Term *t)
{
    if (strcmp(t->meaning, "literal") == 0)
        free_literal(t->content);
    if (strcmp(t->meaning, "constant") == 0)
        free_constant(t->content);
    if (strcmp(t->meaning, "variable") == 0)
        free_variable(t->content);
    if (strcmp(t->meaning, "operator") == 0)
        free_operator(t->content);
    free(t->meaning);
    free(t);
    return;
}

void
free_literal(Literal *l)
{
    free(l);
    return;
}

void
free_constant(Constant *c)
{
    free(c->name);
    free(c);
    return;
}

void
free_variable(Variable *v)
{
    for (int i = 0;i < v->indec;i++)
        free_term(v->index[i]);
    free(v->index);
    free(v->name);
    free(v);
    return;
}

void
free_operator(Operator *o)
{
    for (int i = 0;i < o->argc;i++)
        free_term(o->argv[i]);
    free(o->argv);
    free(o->name);
    free(o);
    return;
}

Term *
copy_term(Term *t)
{
    Term *new = (Term *) malloc(sizeof(Term));

    new->meaning = copy_string(t->meaning);

    if(strcmp(t->meaning, "literal") == 0)
        new->content = copy_literal(t->content);
    if(strcmp(t->meaning, "constant") == 0)
        new->content = copy_constant(t->content);
    if(strcmp(t->meaning, "variable") == 0)
        new->content = copy_variable(t->content);
    if(strcmp(t->meaning, "operator") == 0)
        new->content = copy_operator(t->content);

    return new;
}

Literal *
copy_literal(Literal *l)
{
    return construct_literal(l->value);
}

Constant *
copy_constant(Constant *c)
{
    return construct_constant(c->name, c->upper_limit, c->lower_limit);
}

Variable *
copy_variable(Variable *v)
{
    Term **index = (Term **) malloc(sizeof(Term *) * v->indec);

    for(int i = 0;i < v->indec;i++)
        index[i] = copy_term(v->index[i]);

    return construct_variable(v->name, v->indec, index);
}

Operator *
copy_operator(Operator *o)
{
    Term **argv = (Term **) malloc(sizeof(Term *) * o->argc);

    for(int i = 0;i < o->argc;i++)
        argv[i] = copy_term(o->argv[i]);

    return construct_operator(o->name, o->argc, argv);
}

void
print_term(Term *t)
{
    if (strcmp(t->meaning, "literal") == 0)
        print_literal(t->content);
    if (strcmp(t->meaning, "constant") == 0)
        print_constant(t->content);
    if (strcmp(t->meaning, "variable") == 0)
        print_variable(t->content);
    if (strcmp(t->meaning, "operator") == 0)
        print_operator(t->content);
    return;
}

void
print_literal(Literal *l)
{
    printf("%.6g", l->value);
    return;
}

void
print_constant(Constant *c)
{
    printf("%s", c->name);
    return;
}

void
print_variable(Variable *v)
{
    printf("%s", v->name);

    if (v->indec != 0) {
        printf("_{");
        for (int i = 0;i < v->indec;i++) {
            print_term(v->index[i]);
            if (i != v->indec - 1)
                printf(", ");
        }
        printf("}");
    }
    return;
}

void
print_operator(Operator *o)
{
    if (strcmp(o->name, "imaginary") == 0)
        print_imaginary(o);
    if (strcmp(o->name, "additive_inverse") == 0)
        print_additive_inverse(o);
    if (strcmp(o->name, "multiple_inverse") == 0)
        print_multiple_inverse(o);
    if (strcmp(o->name, "+") == 0)
        print_addition(o);
    if (strcmp(o->name, "-") == 0)
        print_subtract(o);
    if (strcmp(o->name, "*") == 0)
        print_multiply(o);
    if (strcmp(o->name, "/") == 0)
        print_divide(o);
    if (strcmp(o->name, "=") == 0)
        print_equals(o);
    return;
}

void
print_imaginary(Operator *o)
{
    printf("i*");
    print_term(o->argv[0]);
    return;
}

void
print_additive_inverse(Operator *o)
{
    printf("(-");
    print_term(o->argv[0]);
    printf(")");
    return;
}

void
print_multiple_inverse(Operator *o)
{
    printf("(1.0/");
    print_term(o->argv[0]);
    printf(")");
    return;
}

void
print_addition(Operator *o)
{
    printf("(");

    print_term(o->argv[0]);
    printf(" + ");
    print_term(o->argv[1]);

    for(int i = 2;i < o->argc;i++) {
        printf(" + ");
        print_term(o->argv[i]);
    }
    printf(")");
    return;
}

void
print_subtract(Operator *o)
{
    printf("(");
    print_term(o->argv[0]);
    printf(" - ");
    print_term(o->argv[1]);
    printf(")");
    return;
}

void
print_multiply(Operator *o)
{
    printf("(");
    print_term(o->argv[0]);
    printf(" * ");
    print_term(o->argv[1]);

    for(int i = 2;i < o->argc;i++) {
        printf(" * ");
        print_term(o->argv[i]);
    }
    printf(")");
    return;
}

void
print_divide(Operator *o)
{
    printf("(");
    print_term(o->argv[0]);
    printf(") / (");
    print_term(o->argv[1]);
    printf(")");
    return;
}

void
print_equals(Operator *o)
{
    print_term(o->argv[0]);
    printf(" = ");
    print_term(o->argv[1]);
    return;
}

Operator *is_operator(Term *t, char *name)
{
    if (strcmp(t->meaning, "operator") != 0)
        return NULL;

    Operator *o = t->content;

    if (strcmp(o->name, name) != 0)
        return NULL;

    return o;
}

Term *
imaginary(Term *t)
{
    Term **argv = (Term **) malloc(sizeof(Term *));

    argv[0] = t;

    return operator("imaginary", 1, argv);
}

Term *
additive_inverse(Term *t)
{
    Term **argv = (Term **) malloc(sizeof(Term *));

    argv[0] = t;

    return operator("additive_inverse", 1, argv);
}

Term *
multiple_inverse(Term *t)
{
    Term **argv = (Term **) malloc(sizeof(Term *));

    argv[0] = t;

    return operator("multiple_inverse", 1, argv);
}

Term *
add(Term *lhs, Term *rhs)
{
    int lhs_argc;
    int rhs_argc;

    Operator *o_lhs = is_operator(lhs, "+");
    if (o_lhs == NULL)
        lhs_argc = 1;
    else
        lhs_argc = o_lhs->argc;

    Operator *o_rhs = is_operator(rhs, "+");
    if (o_rhs == NULL)
        rhs_argc = 1;
    else
        rhs_argc = o_rhs->argc;

    int i = 0;
    Term **argv = (Term **) malloc(sizeof(Term *) * (lhs_argc + rhs_argc));

    if(o_lhs == NULL) {
        argv[i] = lhs;
        i++;
    } else {
        for(int j = 0;j < o_lhs->argc;j++, i++)
            argv[i] = o_lhs->argv[j];
    }

    if(o_rhs == NULL) {
        argv[i] = rhs;
        i++;
    } else {
        for(int j = 0;j < o_rhs->argc;j++, i++)
            argv[i] = o_rhs->argv[j];
    }

    return operator("+", (lhs_argc + rhs_argc), argv);
}

Term *
subtract(Term *lhs, Term *rhs)
{
    Term **argv = (Term **) malloc(sizeof(Term *) * 2);

    argv[0] = lhs;
    argv[1] = additive_inverse(rhs);

    return operator("+", 2, argv);
}

Term *
multiply(Term *lhs, Term *rhs)
{
    int lhs_argc;
    int rhs_argc;

    Operator *o_lhs = is_operator(lhs, "*");
    if (o_lhs == NULL)
        lhs_argc = 1;
    else
        lhs_argc = o_lhs->argc;

    Operator *o_rhs = is_operator(rhs, "*");
    if (o_rhs == NULL)
        rhs_argc = 1;
    else
        rhs_argc = o_rhs->argc;

    int i = 0;
    Term **argv = (Term **) malloc(sizeof(Term *) * (lhs_argc + rhs_argc));

    if(o_lhs == NULL) {
        argv[i] = lhs;
        i++;
    } else {
        for(int j = 0;j < o_lhs->argc;j++, i++)
            argv[i] = o_lhs->argv[j];
    }

    if(o_rhs == NULL) {
        argv[i] = rhs;
        i++;
    } else {
        for(int j = 0;j < o_rhs->argc;j++, i++)
            argv[i] = o_rhs->argv[j];
    }

    return operator("*", (lhs_argc + rhs_argc), argv);
}

Term *
divide(Term *lhs, Term *rhs)
{
    Term **argv = (Term **) malloc(sizeof(Term *) * 2);

    argv[0] = lhs;
    argv[1] = multiple_inverse(rhs);

    return operator("*", 2, argv);
}

Term *
equal(Term *lhs, Term *rhs)
{
    Term **argv = (Term **) malloc(sizeof(Term *) * 2);

    argv[0] = lhs;
    argv[1] = rhs;

    return operator("=", 2, argv);
}

Term *
simplify(Term *t)
{
    if(strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = t->content;
    for(int i = 0;i < o->argc;i++)
        o->argv[i] = simplify(o->argv[i]);

    Term *simple = t;

    simple = simplify_double_additive_inverse(simple);
    simple = simplify_double_multiple_inverse(simple);
    simple = simplify_double_imaginary(simple);

    simple = simplify_additive_assoziativity(simple);
    simple = simplify_multiple_assoziativity(simple);

    simple = simplify_multiple_additive_inverse(simple);
    simple = simplify_multiple_imaginary(simple);
    simple = simplify_multiple_inverse_imaginary(simple);
    simple = simplify_multiple_inverse_additive_inverse(simple);

    simple = swap_additive_inverse_imaginary(simple);

    simple = simplify_additive_inverse_add(simple);
    simple = simplify_imaginary_add(simple);

    simple = simplify_order_added_terms(simple);
    simple = simplify_order_multiplied_terms(simple);

    simple = simplify_combine_multiplied_multiple_inverse(simple);
    simple = simplify_combine_added_neutral_terms(simple);
    simple = simplify_combine_multiplied_neutral_terms(simple);

    simple = simplify_with_distributive_law(simple);
    simple = simplify_recursive_multiple_inverse(simple);
    simple = simplify_combine_additive_variable_terms(simple);

    simple = simplify_imaginary_zero(simple);
    simple = simplify_additive_inverse_zero(simple);

    simple = simplify_added_zero(simple);
    simple = simplify_multiplied_one(simple);
    simple = simplify_multiplied_zero(simple);
    simple = simplify_multiple_inverse_one(simple);

    simple = add_literals(simple);
    simple = add_imaginary_literals(simple);
    simple = multiply_literals(simple);

//    simple = simplify_partial_fraction_expansion(simple);

    return simple;
}

Term *
simplify_double_additive_inverse(Term *t)
{
    Operator *o = is_operator(t, "additive_inverse");
    if (o == NULL)
        return t;

    Operator *o_1 = is_operator(o->argv[0], "additive_inverse");
    if (o_1 == NULL)
        return t;

    Term *simple = copy_term(o_1->argv[0]);

    free_term(t);

    return simplify(simple);
}

Term *
simplify_double_multiple_inverse(Term *t)
{
    Operator *o = is_operator(t, "multiple_inverse");
    if (o == NULL)
        return t;

    Operator *o_1 = is_operator(o->argv[0], "multiple_inverse");
    if (o_1 == NULL)
        return t;

    Term *simple = copy_term(o_1->argv[0]);

    free_term(t);

    return simplify(simple);
}

Term *
simplify_double_imaginary(Term *t)
{
    Operator *o = is_operator(t, "imaginary");
    if (o == NULL)
        return t;

    Operator *o_1 = is_operator(o->argv[0], "imaginary");
    if (o_1 == NULL)
        return t;

    Term *simple = additive_inverse(copy_term(o_1->argv[0]));

    free_term(t);

    return simplify(simple);
}

Term *
simplify_additive_assoziativity(Term *t)
{
    Operator *o = is_operator(t, "+");
    if (o == NULL)
        return t;

    int i;
    for(i = 0;i < o->argc;i++) {
        Operator *o_1 = is_operator(o->argv[i], "+");
        if (o_1 == NULL)
            continue;

        break;
    }

    if (i >= o->argc)
        return t;

    Term *simple = add(copy_term(o->argv[0]), copy_term(o->argv[1]));

    for (int i = 2;i < o->argc;i++)
        simple = add(simple, copy_term(o->argv[i]));

    free_term(t);

    return simplify(simple);
}

Term *
simplify_multiple_assoziativity(Term *t)
{
    Operator *o = is_operator(t, "*");
    if (o == NULL)
        return t;

    int i;
    for(i = 0;i < o->argc;i++) {
        Operator *o_1 = is_operator(o->argv[i], "*");
        if (o_1 == NULL)
            continue;

        break;
    }

    if (i >= o->argc)
        return t;

    Term *simple = multiply(copy_term(o->argv[0]), copy_term(o->argv[1]));

    for (int i = 2;i < o->argc;i++)
        simple = multiply(simple, copy_term(o->argv[i]));

    free_term(t);

    return simplify(simple);
}

Term *
simplify_multiple_additive_inverse(Term *t)
{
    Operator *o = is_operator(t, "*");
    if (o == NULL)
        return t;

    int i;
    Term *replacement;

    for(i = 0;i < o->argc;i++) {
        Operator *o_1 = is_operator(o->argv[i], "additive_inverse");
        if (o_1 == NULL)
            continue;

        replacement = copy_term(o_1->argv[0]);
        break;
    }

    if (i >= o->argc)
        return t;

    free_term(o->argv[i]);
    o->argv[i] = replacement;

    return simplify(additive_inverse(t));
}

Term *
simplify_multiple_imaginary(Term *t)
{
    Operator *o = is_operator(t, "*");
    if (o == NULL)
        return t;

    int i;
    Term *replacement;

    for(i = 0;i < o->argc;i++) {
        Operator *o_1 = is_operator(o->argv[i], "imaginary");
        if (o_1 == NULL)
            continue;

        replacement = copy_term(o_1->argv[0]);
        break;
    }

    if (i >= o->argc)
        return t;

    free_term(o->argv[i]);
    o->argv[i] = replacement;

    return simplify(imaginary(t));
}

Term *
simplify_multiple_inverse_imaginary(Term *t)
{
    Operator *o = is_operator(t, "multiple_inverse");
    if (o == NULL)
        return t;

    Operator *o_1 = is_operator(o->argv[0], "imaginary");
    if (o_1 == NULL)
        return t;

    Term *simple = imaginary(additive_inverse(multiple_inverse(copy_term(o_1->argv[0]))));

    free_term(t);

    return simplify(simple);
}

Term *
simplify_multiple_inverse_additive_inverse(Term *t)
{
    Operator *o = is_operator(t, "multiple_inverse");
    if (o == NULL)
        return t;

    Operator *o_1 = is_operator(o->argv[0], "additive_inverse");
    if (o_1 == NULL)
        return t;

    Term *simple = additive_inverse(multiple_inverse(copy_term(o_1->argv[0])));

    free_term(t);

    return simplify(simple);
}

Term *swap_additive_inverse_imaginary(Term *t)
{
    Operator *o = is_operator(t, "additive_inverse");
    if (o == NULL)
        return t;

    Operator *o_1 = is_operator(o->argv[0], "imaginary");
    if (o_1 == NULL)
        return t;

    Term *simple = imaginary(additive_inverse(copy_term(o_1->argv[0])));

    free_term(t);

    return simplify(simple);
}

Term *simplify_additive_inverse_add(Term *t)
{
    Operator *o = is_operator(t, "additive_inverse");
    if (o == NULL)
        return t;

    Operator *o_1 = is_operator(o->argv[0], "+");
    if (o_1 == NULL)
        return t;

    for (int i = 0;i < o_1->argc;i++)
        o_1->argv[i] = additive_inverse(o_1->argv[i]);

    Term *simple = copy_term(o->argv[0]);

    free_term(t);

    return simplify(simple);
}

Term *simplify_imaginary_add(Term *t)
{
    Operator *o = is_operator(t, "imaginary");
    if (o == NULL)
        return t;

    Operator *o_1 = is_operator(o->argv[0], "+");
    if (o_1 == NULL)
        return t;

    for (int i = 0;i < o_1->argc;i++)
        o_1->argv[i] = imaginary(o_1->argv[i]);

    Term *simple = copy_term(o->argv[0]);

    free_term(t);

    return simplify(simple);
}

Term *simplify_order_added_terms(Term *t)
{
    Operator *o = is_operator(t, "+");
    if (o == NULL)
        return t;

    merge_sort_added_terms(o->argv, 0, o->argc - 1);

    return t;
}

Term *simplify_order_multiplied_terms(Term *t)
{
    Operator *o = is_operator(t, "*");
    if (o == NULL)
        return t;

    merge_sort_multiplied_terms(o->argv, 0, o->argc - 1);

    return t;
}

Term *
simplify_combine_multiplied_multiple_inverse(Term *t)
{
    Operator *o = is_operator(t, "*");
    if (o == NULL)
        return t;

    int i, j, k;
    Term *t_1;

    for (i = 0;i < o->argc;i++) {
        Operator *o_1 = is_operator(o->argv[i], "multiple_inverse");
        if (o_1 == NULL)
            continue;

        t_1 = o_1->argv[0];
        j = i;
        break;
    }

    if (i >= o->argc)
        return t;

    Term *t_2;

    for (i++;i < o->argc;i++) {
        Operator *o_1 = is_operator(o->argv[i], "multiple_inverse");
        if (o_1 == NULL)
            continue;

        t_2 = o_1->argv[0];
        k = i;
        break;
    }

    if (i >= o->argc)
        return t;

    Term *simple = multiple_inverse(multiply(copy_term(t_1), copy_term(t_2)));

    for (i = 0;i < o->argc;i++) {
        if (i == j || i == k)
            continue;

        simple = multiply(simple, copy_term(o->argv[i]));
    }

    free_term(t);

    return simplify(simple);
}

Term *
simplify_combine_multiplied_neutral_terms(Term *t)
{
    Operator *o = is_operator(t, "*");
    if (o == NULL)
        return t;

    bool can_be_simplified = false;

    for (int i = 0;i < o->argc;i++) {
        Operator *o_1 = is_operator(o->argv[i], "multiple_inverse");
        if (o_1 == NULL)
            continue;
        
        Operator *o_2 = is_operator(o_1->argv[0], "*");
        if (o_2 == NULL) {
            for (int k = 0;k < o->argc;k++) {
                if (is_equal(o->argv[k], o_1->argv[0])) {
                    can_be_simplified = true;
                    free_term(o->argv[k]);
                    free_term(o_1->argv[0]);
                    o->argv[k] = literal(1.0);
                    o_1->argv[0] = literal(1.0);
                    break;
                }
            }
        } else {
            for (int j = 0;j < o_2->argc;j++) {
                int k = 0;
                for (;k < o->argc;k++) {
                    if (is_equal(o->argv[k], o_2->argv[0])) {
                        can_be_simplified = true;
                        free_term(o->argv[k]);
                        free_term(o_2->argv[0]);
                        o->argv[k] = literal(1.0);
                        o_2->argv[0] = literal(1.0);
                        break;
                    }
                }
            }
        }

        o->argv[i] = simplify(o->argv[i]);
    }

    if (!can_be_simplified)
        return t;
    else
        return simplify(t);
}

Term *
simplify_combine_added_neutral_terms(Term *t)
{
    Operator *o = is_operator(t, "+");
    if (o == NULL)
        return t;

    int i, j;

    Operator *o_1;
    Term *t_1;

    for (i = 0;i < o->argc;i++) {
        for (;i < o->argc;i++) {
            o_1 = is_operator(o->argv[i], "additive_inverse");
            if (o_1 == NULL)
                continue;

            t_1 = o_1->argv[0];
            break;
        }
        if (i >= o->argc)
            continue;

        for (j = 0;j < o->argc;j++) {
            if (is_equal(o->argv[j], t_1))
                break;
        }
        if (j >= o->argc)
            continue;

        free_term(o->argv[j]);
        free_term(o->argv[i]);
        o->argv[j] = literal(0.0);
        o->argv[i] = literal(0.0);
    }
    if(i >= o->argc)
        return t;

    return simplify(t);
}

Term *
simplify_with_distributive_law(Term *t)
{
    Operator *o = is_operator(t, "*");
    if (o == NULL)
        return t;

    int i;
    Operator *temp_o;

    for (i = 0;i < o->argc;i++) {
        temp_o = is_operator(o->argv[i], "+");
        if (temp_o == NULL)
            continue;

        if (!is_variable_term(t))
            break;

        if (is_variable_term(o->argv[i]))
            break;
        else
            continue;
    }

    if (i >= o->argc)
        return t;

    Term **part = (Term **) malloc(sizeof(Term*) * temp_o->argc);

    for (int j = 0;j < temp_o->argc;j++)
        part[j] = copy_term(temp_o->argv[j]);

    for (int j = 0;j < temp_o->argc;j++) {
        for (int k = 0;k < o->argc;k++) {
            if (i == k)
                continue;

            part[j] = multiply(part[j], copy_term(o->argv[k]));
        }
    }

    Term *simple = operator("+", temp_o->argc, part);

    free_term(t);

    return simplify(simple);
}

Term *
simplify_recursive_multiple_inverse(Term *t)
{
    Operator *o = is_operator(t, "multiple_inverse");
    if (o == NULL)
        return t;

    Operator *o_1 = is_operator(o->argv[0], "+");
    if (o_1 == NULL)
        return t;

    Term *t_inv;
    int i;
    for (i = 0;i < o_1->argc;i++) {

        t_inv = o_1->argv[i];

        Operator *o_2 = is_operator(t_inv, "imaginary");
        if (o_2 != NULL)
            t_inv = o_2->argv[0];

        Operator *o_3 = is_operator(t_inv, "additive_inverse");
        if (o_3 != NULL)
            t_inv = o_3->argv[0];

        t_inv = has_multiple_inverse(t_inv);
        if (t_inv == NULL)
            continue;

        break;
    }

    if (i >= o_1->argc)
        return t;

    for(int i = 0;i < o_1->argc;i++)
        o_1->argv[i] = simplify(multiply(copy_term(o_1->argv[i]), copy_term(t_inv)));

    return simplify(multiply(copy_term(t_inv), t));
}

Term *
has_multiple_inverse(Term *t)
{
    Operator *o = is_operator(t, "multiple_inverse");
    if (o != NULL)
        return o->argv[0];

    o = is_operator(t, "*");
    if (o != NULL)
    {
        Term *t_inv;
        int i;

        for (i = 0;i < o->argc;i++) {
            Operator *o_1 = is_operator(o->argv[i], "multiple_inverse");
            if (o_1 == NULL)
                continue;

            t_inv = o_1->argv[0];
            break;
        }

        if (i >= o->argc)
            return NULL;

        return t_inv;
    }

    return NULL;
}

Term *
simplify_combine_additive_variable_terms(Term *t)
{
    Operator *o = is_operator(t, "+");
    if (o == NULL)
        return t;

    int i, j;

    Term *non_variable, *variable;
    Term *current_non_variable, *current_variable;

    for (i = 0;i < o->argc;i++) {
        non_variable = get_non_variable_terms_from_multiple(o->argv[i]);
        variable = get_variable_terms_from_multiple(o->argv[i]);

        for(j = i + 1;j < o->argc;j++) {
            current_non_variable = get_non_variable_terms_from_multiple(o->argv[j]);
            current_variable = get_variable_terms_from_multiple(o->argv[j]);

            if (is_equal(variable, current_variable) && is_variable_term(current_variable))
                break;

            free_term(current_non_variable);
            free_term(current_variable);
        }

        if (j >= o->argc) {
            free_term(non_variable);
            free_term(variable);
            continue;
        }

        free_term(current_variable);

        break;
    }

    if (i >= o->argc)
        return t;

    Term *simple = multiply(add(non_variable, current_non_variable), variable);

    for (int k = 0;k < o->argc;k++) {
        if(k == i || k == j)
            continue;

        simple = add(simple, copy_term(o->argv[k]));
    }

    free_term(t);

    return simplify(simple);
}

Term *
simplify_imaginary_zero(Term * t)
{
    Operator *o = is_operator(t, "imaginary");
    if (o == NULL)
        return t;

    if (strcmp(o->argv[0]->meaning, "literal") != 0)
        return t;

    Literal *l = o->argv[0]->content;
    if (l->value != 0.0);
        return t;

    free_term(t);

    return literal(0.0);
}

Term *
simplify_additive_inverse_zero(Term *t)
{
    Operator *o = is_operator(t, "additive_inverse");
    if (o == NULL)
        return t;

    if (strcmp(o->argv[0]->meaning, "literal") != 0)
        return t;

    Literal *l = o->argv[0]->content;
    if (l->value != 0.0);
        return t;

    free_term(t);

    return literal(0.0);
}

Term *
simplify_added_zero(Term *t)
{
    Operator *o = is_operator(t, "+");
    if (o == NULL)
        return t;

    int i;
    for (i = 0;i < o->argc;i++) {
        if (strcmp(o->argv[i]->meaning, "literal") == 0)
            break;
    }

    if (i >= o->argc)
        return t;

    Literal *l = o->argv[i]->content;
    if (l->value != 0.0)
        return t;

    Term *simple;
    int j;

    if (i == 0) {
        simple = copy_term(o->argv[1]);
        j = 1;
    } else {
        simple = copy_term(o->argv[0]);
        j = 0;
    }

    for (int k = 0;k < o->argc;k++) {
        if (i == k || j == k)
            continue;

        simple = add(simple, copy_term(o->argv[k]));
    }

    free_term(t);

    return simplify(simple);
}

Term *
simplify_multiplied_one(Term *t)
{
    Operator *o = is_operator(t, "*");
    if (o == NULL)
        return t;

    int i;
    for (i = 0;i < o->argc;i++) {
        if (strcmp(o->argv[i]->meaning, "literal") == 0)
            break;
    }

    if (i >= o->argc)
        return t;

    Literal *l = o->argv[i]->content;

    if (l->value != 1.0)
        return t;

    Term *simple;

    int j;
    if (i == 0) {
        simple = copy_term(o->argv[1]);
        j = 1;
    } else {
        simple = copy_term(o->argv[0]);
        j = 0;
    }

    for (int k = 0;k < o->argc;k++) {
        if (i == k || j == k)
            continue;

        simple = multiply(simple, copy_term(o->argv[k]));
    }

    free_term(t);

    return simplify(simple);
}

Term *
simplify_multiplied_zero(Term *t)
{
    Operator *o = is_operator(t, "*");
    if (o == NULL)
        return t;

    int i;
    for (i = 0;i < o->argc;i++) {
        if (strcmp(o->argv[i]->meaning, "literal") != 0)
            continue;

        Literal *l = o->argv[i]->content;
        if (l->value != 0.0)
            continue;

        break;
    }

    if (i >= o->argc)
        return t;

    free_term(t);
    return literal(0.0);
}

Term *
simplify_multiple_inverse_one(Term *t)
{
    Operator *o = is_operator(t, "multiple_inverse");
    if (o == NULL)
        return t;

    if (strcmp(o->argv[0]->meaning, "literal") != 0)
        return t;

    Literal *l = o->argv[0]->content;
    if (l->value != 1.0)
        return t;

    free_term(t);

    return literal(1.0);
}

Term *
add_literals(Term *t)
{
    Operator *o = is_operator(t, "+");
    if (o == NULL)
        return t;

    int i_lhs, i_rhs;
    double lhs, rhs;

    for (i_lhs = 0;i_lhs < o->argc;i_lhs++) {
        Term *t_1 = o->argv[i_lhs];

        if (strcmp(t_1->meaning, "literal") == 0) {
            Literal *l_1 = t_1->content;
            lhs = l_1->value;
            break;
        }
        if (strcmp(t_1->meaning, "constant") == 0)
            continue;
        if (strcmp(t_1->meaning, "variable") == 0)
            continue;
        if (strcmp(t_1->meaning, "operator") == 0) {
            Operator *o_2 = t_1->content;
            if (strcmp(o_2->name, "additive_inverse") != 0)
                continue;

            Term *t_2 = o_2->argv[0];
            if (strcmp(t_2->meaning, "literal") != 0)
                continue;

            Literal *l_1 = t_2->content;
            lhs = -l_1->value;
            break;
        }
    }
    if(i_lhs >= o->argc)
        return t;

    for (i_rhs = 0;i_rhs < o->argc;i_rhs++) {
        if (i_lhs == i_rhs)
            continue;

        Term *t_1 = o->argv[i_rhs];

        if (strcmp(t_1->meaning, "literal") == 0) {
            Literal *l_1 = t_1->content;
            rhs = l_1->value;
            break;
        }
        if (strcmp(t_1->meaning, "constant") == 0)
            continue;
        if (strcmp(t_1->meaning, "variable") == 0)
            continue;
        if (strcmp(t_1->meaning, "operator") == 0) {
            Operator *o_2 = t_1->content;
            if (strcmp(o_2->name, "additive_inverse") != 0)
                continue;

            Term *t_2 = o_2->argv[0];
            if (strcmp(t_2->meaning, "literal") != 0)
                continue;

            Literal *l_1 = t_2->content;
            rhs = -l_1->value;
            break;
        }
    }
    if(i_rhs >= o->argc)
        return t;

    Term *simple = literal(lhs + rhs);

    for(int i = 0;i < o->argc;i++) {
        if (i == i_lhs || i == i_rhs)
            continue;

        simple = add(simple, copy_term(o->argv[i]));
    }

    free_term(t);

    return simplify(simple);
}

Term *
add_imaginary_literals(Term *t)
{
    Operator *o = is_operator(t, "+");
    if (o == NULL)
        return t;

    int i_lhs, i_rhs;
    double lhs, rhs;

    for (i_lhs = 0;i_lhs < o->argc;i_lhs++) {
        Operator *o_1 = is_operator(o->argv[i_lhs], "imaginary");
        if (o_1 == NULL)
            continue;

        Term *t_1 = o_1->argv[0];

        if (strcmp(t_1->meaning, "literal") == 0) {
            Literal *l_1 = t_1->content;
            lhs = l_1->value;
            break;
        }
        if (strcmp(t_1->meaning, "constant") == 0)
            continue;
        if (strcmp(t_1->meaning, "variable") == 0)
            continue;
        if (strcmp(t_1->meaning, "operator") == 0) {
            Operator *o_2 = t_1->content;
            if (strcmp(o_2->name, "additive_inverse") != 0)
                continue;

            Term *t_2 = o_2->argv[0];
            if (strcmp(t_2->meaning, "literal") != 0)
                continue;

            Literal *l_1 = t_2->content;
            lhs = -l_1->value;
            break;
        }
    }

    if(i_lhs >= o->argc)
        return t;

    for (i_rhs = 0;i_rhs < o->argc;i_rhs++) {
        if (i_lhs == i_rhs)
            continue;

        Operator *o_1 = is_operator(o->argv[i_rhs], "imaginary");
        if (o_1 == NULL)
            continue;

        Term *t_1 = o_1->argv[0];

        if (strcmp(t_1->meaning, "literal") == 0) {
            Literal *l_1 = t_1->content;
            rhs = l_1->value;
            break;
        }
        if (strcmp(t_1->meaning, "constant") == 0)
            continue;
        if (strcmp(t_1->meaning, "variable") == 0)
            continue;
        if (strcmp(t_1->meaning, "operator") == 0) {
            Operator *o_2 = t_1->content;
            if (strcmp(o_2->name, "additive_inverse") != 0)
                continue;

            Term *t_2 = o_2->argv[0];
            if (strcmp(t_2->meaning, "literal") != 0)
                continue;

            Literal *l_1 = t_2->content;
            rhs = -l_1->value;
            break;
        }
    }

    if(i_rhs >= o->argc)
        return t;

    Term *simple = imaginary(literal(lhs + rhs));

    for(int i = 0;i < o->argc;i++) {
        if (i == i_lhs || i == i_rhs)
            continue;

        simple = add(simple, copy_term(o->argv[i]));
    }

    free_term(t);

    return simplify(simple);
}

Term *
multiply_literals(Term *t)
{
    Operator *o = is_operator(t, "*");
    if (o == NULL)
        return t;

    int i_lhs, i_rhs;
    Literal *lhs, *rhs;

    for (i_lhs = 0;i_lhs < o->argc;i_lhs++) {
        if (strcmp(o->argv[i_lhs]->meaning, "literal") != 0)
            continue;

        lhs = o->argv[i_lhs]->content;
        break;
    }

    if(i_lhs >= o->argc)
        return t;

    for (i_rhs = 0;i_rhs < o->argc;i_rhs++) {
        if (i_lhs == i_rhs)
            continue;

        if (strcmp(o->argv[i_rhs]->meaning, "literal") != 0)
            continue;

        rhs = o->argv[i_rhs]->content;
        break;
    }

    if(i_rhs >= o->argc)
        return t;

    Term *simple = literal(lhs->value * rhs->value);

    for(int i = 0;i < o->argc;i++) {
        if (i == i_lhs || i == i_rhs)
            continue;

        simple = multiply(simple, copy_term(o->argv[i]));
    }

    free_term(t);

    return simplify(simple);
}

Term *
simplify_partial_fraction_expansion(Term *t)
{
    return t;
}

int main()
{
// functions:
// c ... simplify_partial_fraction_expansion(term)

// x ... simplify(term)
// o ... solve(term) // for differential equations
// o ... solve(term, variable)
// o ... minimize(term, variable, ...)
// o ... maximize(term, variable, ...)
// o ... series(term, variable, ...)
// x ... print(term)
// o ... pretty_print(term)
// o ... plot(term, variable)
// o ... plot(term, variable, variable)
// o ... plot(term, variable, variable, variable)

// operators:
// i .......... imaginary ............. {term}
// negative ... negative .............. {term}
// + .......... add ................... {term, ...}
// - .......... subtract .............. {term, term}
// * .......... multiply .............. {term, ...}
// / .......... divide ................ {term, term}
// = .......... equal ................. {term, term}

    Term *x = variable("x");
    Term *a = literal(2.0);
    Term *b = literal(3.0);

    Term *c = multiple_inverse(add(additive_inverse(multiple_inverse(copy_term(a))), imaginary(multiple_inverse(copy_term(x)))));

    print_term(c);
    printf("\n");

    print_term(simplify(c));
    printf("\n");

    return 0x00;
}
