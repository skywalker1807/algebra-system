#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

typedef enum { false, true } bool;

extern int errno;

typedef struct Term Term;
typedef struct Variable Variable;
typedef struct Literal Literal;
typedef struct Constant Constant;
typedef struct Operator Operator;

struct Term {
    void *content;
    char *meaning;
};

struct Variable {
    char *name;
    int indec;
    Term **index;
};

struct Literal {
    double value;
};

struct Constant {
    char *name;
    double upper_limit;
    double lower_limit;
};

struct Operator {
    char *name;
    int argc;
    Term **argv;
};

static char* copy_string(char* str);

static Term *term(void *content, char* meaning);
static Term *variable(char *name);
static Term *variable_with_index(char *name, int indec, Term **index);
static Term *literal(double value);
static Term *constant(char *name, double upper_limit, double lower_limit);
struct Term *operator(char *name, int argc, struct Term **argv);

static Variable *construct_variable(char *name, int indec, Term **index);
static Literal *construct_literal(double value);
static Constant *construct_constant(char *name, double upper_limit, double lower_limit);
static Operator *construct_operator(char *name, int argc, Term **argv);

static void free_term(Term *t);
static void free_variable(Variable *v);
static void free_literal(Literal *l);
static void free_constant(Constant *c);
static void free_operator(Operator *o);

static Term *copy_term(Term *t);
static Variable *copy_variable(Variable *v);
static Literal *copy_literal(Literal *l);
static Constant *copy_constant(Constant *c);
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

static Term *imaginary(Term *t);
static Term *additive_inverse(Term *t);
static Term *multiple_inverse(Term *t);
static Term *add(Term *lhs, Term *rhs);
static Term *subtract(Term *lhs, Term *rhs);
static Term *multiply(Term *lhs, Term *rhs);
static Term *divide(Term *lhs, Term *rhs);
static Term *equal(Term *lhs, Term *rhs);

static Term *simplify(Term *t);
static Term *simplify_subtraction(Term *t);
static Term *simplify_division(Term *t);
static Term *simplify_double_additive_inverse(Term *t);
static Term *simplify_double_multiple_inverse(Term *t);
static Term *simplify_double_imaginary(Term *t);
static Term *simplify_additive_assoziativity(Term *t);
static Term *simplify_multiple_assoziativity(Term *t);
static Term *simplify_multiple_negative(Term *t);
static Term *simplify_multiple_imaginary(Term *t);
static Term *swap_negative_imaginary(Term *t);
static Term *simplify_additive_inverse_add(Term *t);
static Term *simplify_imaginary_add(Term *t);
static Term *simplify_with_distributive_law(Term *t);

static Term *add_literals(Term *t);
static Term *add_imaginary_literals(Term *t);
static Term *multiply_literals(Term *t);

static Term *order_multiplied_terms(Term *t);

static bool is_less(Term *lhs, Term *rhs);
static bool is_greater(Term *lhs, Term *rhs);
static bool is_equal(Term *lhs, Term *rhs);

static void merge_sort_terms(Term *arr[], int l, int r);
static void merge_terms(Term *arr[], int l, int m, int r);

static bool is_variable_term(Term* t);
static Term *get_variable_terms_from_multiple(Term* t);
static Term *get_non_variable_terms_from_multiple(Term* t);
static Term *combine_additive_variable_terms(Term* t);

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

Variable *
construct_variable(char *name, int indec, Term **index)
{
    Variable* v = (Variable *) malloc(sizeof(Variable));

    v->name = copy_string(name);
    v->indec = indec;
    v->index = index;

    return v;
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

Operator *
construct_operator(char *name, int argc, Term **argv)
{
    Operator *o = (Operator *) malloc(sizeof(Operator));

    o->name = copy_string(name);
    o->argc = argc;
    o->argv = argv;

    return o;
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
operator(char *name, int argc, Term **argv)
{
    Operator *o = construct_operator(name, argc, argv);

    return term(o, "operator");
}

void
free_term(Term *t)
{
    if (strcmp(t->meaning, "variable") == 0)
        free_variable(t->content);
    if (strcmp(t->meaning, "literal") == 0)
        free_literal(t->content);
    if (strcmp(t->meaning, "constant") == 0)
        free_constant(t->content);
    if (strcmp(t->meaning, "operator") == 0)
        free_operator(t->content);
    free(t->meaning);
    free(t);
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

    if(strcmp(t->meaning, "operator") == 0)
        new->content = copy_operator(t->content);
    if(strcmp(t->meaning, "variable") == 0)
        new->content = copy_variable(t->content);
    if(strcmp(t->meaning, "literal") == 0)
        new->content = copy_literal(t->content);
    if(strcmp(t->meaning, "constant") == 0)
        new->content = copy_constant(t->content);

    return new;
}

Variable *
copy_variable(Variable *v)
{
    Term **index = (Term **) malloc(sizeof(Term *) * v->indec);

    for(int i = 0;i < v->indec;i++)
        index[i] = copy_term(v->index[i]);

    return construct_variable(v->name, v->indec, index);
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
    if (t == NULL)
    return;

    if (strcmp(t->meaning, "constant") == 0)
        print_constant(t->content);
    if (strcmp(t->meaning, "literal") == 0)
        print_literal(t->content);
    if (strcmp(t->meaning, "variable") == 0)
        print_variable(t->content);
    if (strcmp(t->meaning, "operator") == 0)
        print_operator(t->content);
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

Term *
imaginary(Term *t)
{
    Term **argv = (Term **) malloc(sizeof(Term *));

    argv[0] = copy_term(t);

    return operator("imaginary", 1, argv);
}

Term *
additive_inverse(Term *t)
{
    Term **argv = (Term **) malloc(sizeof(Term *));

    argv[0] = copy_term(t);

    return operator("additive_inverse", 1, argv);
}

Term *
multiple_inverse(Term *t)
{
    Term **argv = (Term **) malloc(sizeof(Term *));

    argv[0] = copy_term(t);

    return operator("multiple_inverse", 1, argv);
}

Term *
add(Term *lhs, Term *rhs)
{
    bool lhs_add;
    bool rhs_add;

    int lhs_argc;
    int rhs_argc;

    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(((Operator *) lhs->content)->name, "+") == 0) {
        lhs_argc = ((Operator *) lhs->content)->argc;
        lhs_add = true;
    } else {
        lhs_argc = 1;
        lhs_add = false;
    }

    if (strcmp(rhs->meaning, "operator") == 0 &&
        strcmp(((Operator *) rhs->content)->name, "+") == 0) {
        rhs_argc = ((Operator *) rhs->content)->argc;
        rhs_add = true;
    } else {
        rhs_argc = 1;
        rhs_add = false;
    }

    int argc = lhs_argc + rhs_argc;

    int i = 0;
    Term **argv = (Term **) malloc(sizeof(Term *) * argc);

    if(lhs_add)
        for(int j = 0;j < lhs_argc;i++, j++)
            argv[i] = ((Operator *) lhs->content)->argv[j];
    else {
        argv[i] = lhs;
        i++;
    }

    if(rhs_add)
        for(int j = 0;j < rhs_argc;i++, j++)
            argv[i] = ((Operator *) rhs->content)->argv[j];
    else {
        argv[i] = rhs;
        i++;
    }

    return operator("+", argc, argv);
}

Term *
subtract(Term *lhs, Term *rhs)
{
    Term **argv = (Term **) malloc(sizeof(Term *) * 2);

    argv[0] = copy_term(lhs);
    argv[1] = copy_term(rhs);

    return operator("-", 2, argv);
}

Term *
multiply(Term *lhs, Term *rhs)
{
    bool lhs_add;
    bool rhs_add;

    int lhs_argc;
    int rhs_argc;

    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(((Operator *) lhs->content)->name, "*") == 0) {
        lhs_argc = ((Operator *) lhs->content)->argc;
        lhs_add = true;
    } else {
        lhs_argc = 1;
        lhs_add = false;
    }

    if (strcmp(rhs->meaning, "operator") == 0 &&
        strcmp(((Operator *) rhs->content)->name, "*") == 0) {
        rhs_argc = ((Operator *) rhs->content)->argc;
        rhs_add = true;
    } else {
        rhs_argc = 1;
        rhs_add = false;
    }

    int argc = lhs_argc + rhs_argc;

    int i = 0;
    Term **argv = (Term **) malloc(sizeof(Term *) * argc);

    if(lhs_add)
        for(int j = 0;j < lhs_argc;i++, j++)
            argv[i] = ((Operator *) lhs->content)->argv[j];
    else {
        argv[i] = lhs;
        i++;
    }

    if(rhs_add)
        for(int j = 0;j < rhs_argc;i++, j++)
            argv[i] = ((Operator *) rhs->content)->argv[j];
    else {
        argv[i] = rhs;
        i++;
    }

    return operator("*", argc, argv);
}

Term *
divide(Term *lhs, Term *rhs)
{
    Term **argv = (Term **) malloc(sizeof(Term *) * 2);

    argv[0] = copy_term(lhs);
    argv[1] = copy_term(rhs);

    return operator("/", 2, argv);
}

Term *
equal(Term *lhs, Term *rhs)
{
    Term **argv = (Term **) malloc(sizeof(Term *) * 2);

    argv[0] = copy_term(lhs);
    argv[1] = copy_term(rhs);

    return operator("=", 2, argv);
}

Term *
simplify(Term *t)
{
    if(strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;

    for(int i = 0;i < o->argc;i++)
        o->argv[i] = simplify(o->argv[i]);

    Term *simple = t;

    simple = simplify_subtraction(simple);
    simple = simplify_division(simple);

    simple = simplify_double_additive_inverse(simple);

    simple = simplify_double_multiple_inverse(simple);
    simple = simplify_double_imaginary(simple);

    simple = simplify_additive_assoziativity(simple);
    simple = simplify_multiple_assoziativity(simple);

    simple = simplify_multiple_negative(simple);
    simple = simplify_multiple_imaginary(simple);

    simple = swap_negative_imaginary(simple);
    simple = simplify_additive_inverse_add(simple);
    simple = simplify_imaginary_add(simple);


    simple = simplify_with_distributive_law(simple);

    simple = add_literals(simple);
    simple = add_imaginary_literals(simple);
    simple = multiply_literals(simple);

    simple = order_multiplied_terms(simple);
//    simple = order_added_terms(simple);

    simple = combine_additive_variable_terms(simple);
    return simple;
}

Term *
simplify_subtraction(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = t->content;
    if (strcmp(o->name, "-") != 0)
        return t;

    Term *simple = add(copy_term(o->argv[0]), additive_inverse(o->argv[1]));
    free_term(t);

    return simplify(simple);
}

Term *
simplify_division(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = t->content;
    if (strcmp(o->name, "/") != 0)
        return t;

    Term *simple = multiply(copy_term(o->argv[0]), multiple_inverse(o->argv[1]));
    free_term(t);

    return simplify(simple);
}

Term *
simplify_double_additive_inverse(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "additive_inverse") != 0)
        return t;

    Term *t_1 = o->argv[0];
    if (strcmp(t_1->meaning, "operator") != 0)
        return t;

    Operator *o_1 = (Operator *) t_1->content;
    if (strcmp(o_1->name, "additive_inverse") != 0)
        return t;

    Term *simple = copy_term(o_1->argv[0]);
    free(t);

    return simplify(simple);
}

Term *
simplify_double_multiple_inverse(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "multiple_inverse") != 0)
        return t;

    Term *t_1 = o->argv[0];
    if (strcmp(t_1->meaning, "operator") != 0)
        return t;

    Operator *o_1 = (Operator *) t_1->content;
    if (strcmp(o_1->name, "multiple_inverse") != 0)
        return t;

    Term *simple = copy_term(o_1->argv[0]);
    free(t);

    return simplify(simple);
}

Term *
simplify_double_imaginary(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "imaginary") != 0)
        return t;

    Term *t_1 = o->argv[0];
    if (strcmp(t_1->meaning, "operator") != 0)
        return t;

    Operator *o_1 = (Operator *) t_1->content;
    if (strcmp(o_1->name, "imaginary") != 0)
        return t;

    Term *simple = additive_inverse(o_1->argv[0]);
    free_term(t);

    return simplify(simple);
}

Term *
simplify_multiple_negative(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "*") != 0)
        return t;

    int i;
    Term *replacement = NULL;

    for(i = 0;i < o->argc;i++) {
        if (strcmp(o->argv[i]->meaning, "operator") != 0)
            continue;

        Operator *temp = (Operator *) o->argv[i]->content;
        if (strcmp(temp->name, "additive_inverse") != 0)
            continue;

        replacement = copy_term(temp->argv[0]);
        break;
    }

    if (i >= o->argc)
        return t;

    free_term(o->argv[i]);
    o->argv[i] = replacement;

    Term *simple = additive_inverse(t);
    free_term(t);

    return simplify(simple);
}

Term *
simplify_multiple_imaginary(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "*") != 0)
        return t;

    int i;
    Term *replacement = NULL;

    for(i = 0;i < o->argc;i++) {
        if (strcmp(o->argv[i]->meaning, "operator") != 0)
            continue;

        Operator *temp = (Operator *) o->argv[i]->content;
        if (strcmp(temp->name, "imaginary") != 0)
            continue;

        replacement = copy_term(temp->argv[0]);
        break;
    }

    if (i >= o->argc)
        return t;

    free_term(o->argv[i]);
    o->argv[i] = replacement;

    Term *simple = imaginary(t);
    free_term(t);

    return simplify(simple);
}

Term *
swap_negative_imaginary(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "additive_inverse") != 0)
        return t;

    Term *t_1 = o->argv[0];
    if (strcmp(t_1->meaning, "operator") != 0)
        return t;

    Operator *o_1 = (Operator *) t_1->content;
    if (strcmp(o_1->name, "imaginary") != 0)
        return t;

    Term *temp = additive_inverse(o_1->argv[0]);
    Term *simple = imaginary(temp);

    free_term(temp);
    free_term(t);

    return simplify(simple);
}

Term *
simplify_additive_assoziativity(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "+") != 0)
        return t;

    bool found = false;
    for(int i = 0;i < o->argc;i++) {
        if (strcmp(o->argv[i]->meaning, "operator") != 0)
            continue;

        Operator *t_1 = (Operator *) o->argv[i]->content;
        if (strcmp(t_1->name, "+") != 0)
            continue;

        found = true;
        break;
    }

    if (!found)
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
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "*") != 0)
        return t;

    bool found = false;
    for(int i = 0;i < o->argc;i++) {
        if (strcmp(o->argv[i]->meaning, "operator") != 0)
            continue;

        Operator *t_1 = (Operator *) o->argv[i]->content;
        if (strcmp(t_1->name, "*") != 0)
            continue;

        found = true;
        break;
    }

    if (!found)
        return t;

    Term *simple = multiply(copy_term(o->argv[0]), copy_term(o->argv[1]));

    for (int i = 2;i < o->argc;i++)
        simple = multiply(simple, copy_term(o->argv[i]));

    free_term(t);

    return simplify(simple);
}

Term *
simplify_additive_inverse_add(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "additive_inverse") != 0)
        return t;

    if (strcmp(o->argv[0]->meaning, "operator") != 0)
        return t;

    Operator *o_1 = (Operator *) o->argv[0]->content;
    if (strcmp(o_1->name, "+") != 0)
        return t;

    Term *simple = add(additive_inverse(o_1->argv[0]), additive_inverse(o_1->argv[1]));

    for (int i = 2;i < o_1->argc;i++)
        simple = add(simple, additive_inverse(o_1->argv[i]));

    free_term(t);

    return simplify(simple);
}

Term *
simplify_imaginary_add(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "imaginary") != 0)
        return t;

    if (strcmp(o->argv[0]->meaning, "operator") != 0)
        return t;

    Operator *o_1 = (Operator *) o->argv[0]->content;
    if (strcmp(o_1->name, "+") != 0)
        return t;

    Term *simple = add(imaginary(o_1->argv[0]), imaginary(o_1->argv[1]));

    for (int i = 2;i < o_1->argc;i++)
        simple = add(simple, imaginary(o_1->argv[i]));

    free_term(t);

    return simplify(simple);
}

Term *
simplify_with_distributive_law(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "*") != 0)
        return t;

    Operator *temp_o;

    int i;

    for (i = 0;i < o->argc;i++) {
        Term *temp_t = o->argv[i];
        if (strcmp(temp_t->meaning, "operator") != 0)
            continue;

        temp_o = (Operator *) temp_t->content;
        if (strcmp(temp_o->name, "+") != 0)
            continue;
        break;
    }

    if (i >= o->argc)
        return t;

    Term *part[temp_o->argc];

    for (int j = 0;j < temp_o->argc;j++)
        part[j] = copy_term(temp_o->argv[j]);

    for (int j = 0;j < temp_o->argc;j++) {
        for (int k = 0;k < o->argc;k++) {
            if (i == k)
                continue;
            part[j] = multiply(part[j], copy_term(o->argv[k]));
        }
    }

    Term *simple = add(part[0], part[1]);
    for (int j = 2;j < temp_o->argc;j++)
        simple = add(simple, part[j]);

    free(t);

    return simplify(simple);
}

Term *
add_literals(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "+") != 0)
        return t;

    int i_lhs, i_rhs;
    double lhs, rhs;

    for (i_lhs = 0;i_lhs < o->argc;i_lhs++) {
        Term *t_1 = (Term *) o->argv[i_lhs];

        if (strcmp(t_1->meaning, "literal") == 0) {
            Literal *l_1 = (Literal *) t_1->content;
            lhs = l_1->value;
            break;
        }
        if (strcmp(t_1->meaning, "constant") == 0)
            continue;
        if (strcmp(t_1->meaning, "variable") == 0)
            continue;
        if (strcmp(t_1->meaning, "operator") == 0) {
            Operator *o_2 = (Operator *) t_1->content;
            if (strcmp(o_2->name, "additive_inverse") != 0)
                continue;

            Term *t_2 = (Term *) o_2->argv[0];
            if (strcmp(t_2->meaning, "literal") != 0)
                continue;

            Literal *l_1 = (Literal *) t_2->content;
            lhs = -l_1->value;
            break;
        }
    }
    if(i_lhs >= o->argc)
        return t;

    for (i_rhs = 0;i_rhs < o->argc;i_rhs++) {
        if (i_lhs == i_rhs)
            continue;

        Term *t_1 = (Term *) o->argv[i_rhs];

        if (strcmp(t_1->meaning, "literal") == 0) {
            Literal *l_1 = (Literal *) t_1->content;
            rhs = l_1->value;
            break;
        }
        if (strcmp(t_1->meaning, "constant") == 0)
            continue;
        if (strcmp(t_1->meaning, "variable") == 0)
            continue;
        if (strcmp(t_1->meaning, "operator") == 0) {
            Operator *o_2 = (Operator *) t_1->content;
            if (strcmp(o_2->name, "additive_inverse") != 0)
                continue;

            Term *t_2 = (Term *) o_2->argv[0];
            if (strcmp(t_2->meaning, "literal") != 0)
                continue;

            Literal *l_1 = (Literal *) t_2->content;
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
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "+") != 0)
        return t;

    int i_lhs, i_rhs;
    double lhs, rhs;

    for (i_lhs = 0;i_lhs < o->argc;i_lhs++) {
        Term *current = o->argv[i_lhs];
        if (strcmp(current->meaning, "operator") != 0)
            continue;

        Operator *o_1 = (Operator *) current->content;
        if (strcmp(o_1->name, "imaginary") != 0)
            continue;

        Term *t_1 = (Term *) o_1->argv[0];

        if (strcmp(t_1->meaning, "literal") == 0) {
            Literal *l_1 = (Literal *) t_1->content;
            lhs = l_1->value;
            break;
        }
        if (strcmp(t_1->meaning, "constant") == 0)
            continue;
        if (strcmp(t_1->meaning, "variable") == 0)
            continue;
        if (strcmp(t_1->meaning, "operator") == 0) {
            Operator *o_2 = (Operator *) t_1->content;
            if (strcmp(o_2->name, "additive_inverse") != 0)
                continue;

            Term *t_2 = (Term *) o_2->argv[0];
            if (strcmp(t_2->meaning, "literal") != 0)
                continue;

            Literal *l_1 = (Literal *) t_2->content;
            lhs = -l_1->value;
            break;
        }
    }
    if(i_lhs >= o->argc)
        return t;

    for (i_rhs = 0;i_rhs < o->argc;i_rhs++) {
        if (i_lhs == i_rhs)
            continue;

        Term *current = o->argv[i_rhs];
        if (strcmp(current->meaning, "operator") != 0)
            continue;

        Operator *o_1 = (Operator *) current->content;
        if (strcmp(o_1->name, "imaginary") != 0)
            continue;

        Term *t_1 = (Term *) o_1->argv[0];

        if (strcmp(t_1->meaning, "literal") == 0) {
            Literal *l_1 = (Literal *) t_1->content;
            rhs = l_1->value;
            break;
        }
        if (strcmp(t_1->meaning, "constant") == 0)
            continue;
        if (strcmp(t_1->meaning, "variable") == 0)
            continue;
        if (strcmp(t_1->meaning, "operator") == 0) {
            Operator *o_2 = (Operator *) t_1->content;
            if (strcmp(o_2->name, "additive_inverse") != 0)
                continue;

            Term *t_2 = (Term *) o_2->argv[0];
            if (strcmp(t_2->meaning, "literal") != 0)
                continue;

            Literal *l_1 = (Literal *) t_2->content;
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
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "*") != 0)
        return t;

    int i_lhs, i_rhs;
    Literal *lhs, *rhs;

    for (i_lhs = 0;i_lhs < o->argc;i_lhs++) {

        if (strcmp(o->argv[i_lhs]->meaning, "literal") != 0)
            continue;

        lhs = (Literal *) o->argv[i_lhs]->content;
        break;
    }

    if(i_lhs >= o->argc)
        return t;

    for (i_rhs = 0;i_rhs < o->argc;i_rhs++) {
        if (i_lhs == i_rhs)
            continue;

        if (strcmp(o->argv[i_rhs]->meaning, "literal") != 0)
            continue;

        rhs = (Literal *) o->argv[i_rhs]->content;
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

static Term *order_multiplied_terms(Term *t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "*") != 0)
        return t;

    merge_sort_terms(o->argv, 0, o->argc - 1);

    return t;
}

bool
is_less(Term *lhs, Term *rhs)
{
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "literal") == 0) {
        Literal *lhs_l = (Literal*) lhs->content;
        Literal *rhs_l = (Literal*) rhs->content;

        if (lhs_l->value < rhs_l->value)
            return true;
        else
            return false;
    }

    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "constant") == 0) {
        Constant *lhs_l = (Constant*) lhs->content;
        Constant *rhs_l = (Constant*) rhs->content;

        if (strcmp(lhs_l->name, rhs_l->name) < 0)
            return true;
        else
            return false;
    }

    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "variable") == 0) {
        Variable *lhs_v = (Variable*) lhs->content;
        Variable *rhs_v = (Variable*) rhs->content;

        if (strcmp(lhs_v->name, rhs_v->name) < 0)
            return true;
        else if (strcmp(lhs_v->name, rhs_v->name) > 0)
            return false;
        else {
            if (lhs_v->indec < rhs_v->indec)
                return true;
            else if (lhs_v->indec > rhs_v->indec)
                return false;
            else {
                for (int i = 0;i < lhs_v->indec;i++) {
                    if (is_less(lhs_v->index[i], rhs_v->index[i]))
                        return true;
                    else if (is_greater(lhs_v->index[i], rhs_v->index[i]))
                        return false;
                    else
                        continue;
                }
                return false;
            }
        }
    }

    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "operator") == 0) {
        Operator *lhs_l = (Operator*) lhs->content;
        Operator *rhs_l = (Operator*) rhs->content;

        // TODO: do something here because of additive variable terms ordering

        if (strcmp(lhs_l->name, rhs_l->name) < 0)
            return true;
        else
            return false;
    }

    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "constant") == 0)
        return true;
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "variable") == 0)
        return true;
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "operator") == 0)
        return true;
    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "variable") == 0)
        return true;
    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "operator") == 0)
        return true;
    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "operator") == 0)
        return true;

    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "literal") == 0)
        return false;
    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "literal") == 0)
        return false;
    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "literal") == 0)
        return false;
    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "constant") == 0)
        return false;
    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "constant") == 0)
        return false;
    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "variable") == 0)
        return false;

    return false;
}

bool
is_greater(Term *lhs, Term *rhs)
{
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "literal") == 0) {
        Literal *lhs_l = (Literal*) lhs->content;
        Literal *rhs_l = (Literal*) rhs->content;

        if (lhs_l->value > rhs_l->value)
            return true;
        else
            return false;
    }

    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "constant") == 0) {
        Constant *lhs_l = (Constant*) lhs->content;
        Constant *rhs_l = (Constant*) rhs->content;

        if (strcmp(lhs_l->name, rhs_l->name) > 0)
            return true;
        else
            return false;
    }

    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "variable") == 0) {
        Variable *lhs_v = (Variable*) lhs->content;
        Variable *rhs_v = (Variable*) rhs->content;

        if (strcmp(lhs_v->name, rhs_v->name) > 0)
            return true;
        else if (strcmp(lhs_v->name, rhs_v->name) < 0)
            return false;
        else {
            if (lhs_v->indec > rhs_v->indec)
                return true;
            else if (lhs_v->indec < rhs_v->indec)
                return false;
            else {
                for (int i = 0;i < lhs_v->indec;i++) {
                    if (is_greater(lhs_v->index[i], rhs_v->index[i]))
                        return true;
                    else if (is_less(lhs_v->index[i], rhs_v->index[i]))
                        return false;
                    else
                        continue;
                }
                return false;
            }
        }
    }

    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "operator") == 0) {
        Operator *lhs_l = (Operator*) lhs->content;
        Operator *rhs_l = (Operator*) rhs->content;

        if (strcmp(lhs_l->name, rhs_l->name) > 0)
            return true;
        else
            return false;
    }

    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "constant") == 0)
        return false;
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "variable") == 0)
        return false;
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "operator") == 0)
        return false;
    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "variable") == 0)
        return false;
    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "operator") == 0)
        return false;
    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "operator") == 0)
        return false;

    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "literal") == 0)
        return true;
    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "literal") == 0)
        return true;
    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "literal") == 0)
        return true;
    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "constant") == 0)
        return true;
    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "constant") == 0)
        return true;
    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "variable") == 0)
        return true;

    return false;
}

bool
is_equal(Term *lhs, Term *rhs)
{
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "literal") == 0) {
        Literal *lhs_l = (Literal*) lhs->content;
        Literal *rhs_l = (Literal*) rhs->content;

        if (lhs_l->value == rhs_l->value)
            return true;
        else
            return false;
    }

    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "constant") == 0) {
        Constant *lhs_l = (Constant*) lhs->content;
        Constant *rhs_l = (Constant*) rhs->content;

        if (strcmp(lhs_l->name, rhs_l->name) == 0)
            return true;
        else
            return false;
    }

    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "variable") == 0) {
        Variable *lhs_v = (Variable*) lhs->content;
        Variable *rhs_v = (Variable*) rhs->content;

        if (strcmp(lhs_v->name, rhs_v->name) > 0)
            return false;
        else if (strcmp(lhs_v->name, rhs_v->name) < 0)
            return false;
        else {
            if (lhs_v->indec > rhs_v->indec)
                return false;
            else if (lhs_v->indec < rhs_v->indec)
                return false;
            else {
                for (int i = 0;i < lhs_v->indec;i++) {
                    if (is_greater(lhs_v->index[i], rhs_v->index[i]))
                        return false;
                    else if (is_less(lhs_v->index[i], rhs_v->index[i]))
                        return false;
                    else
                        continue;
                }
                return true;
            }
        }
    }

    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "operator") == 0) {
        Operator *lhs_l = (Operator*) lhs->content;
        Operator *rhs_l = (Operator*) rhs->content;

        if (strcmp(lhs_l->name, rhs_l->name) == 0)
            return true;
        else
            return false;
    }

    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "constant") == 0)
        return false;
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "variable") == 0)
        return false;
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "operator") == 0)
        return false;
    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "variable") == 0)
        return false;
    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "operator") == 0)
        return false;
    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "operator") == 0)
        return false;

    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "literal") == 0)
        return false;
    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "literal") == 0)
        return false;
    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "literal") == 0)
        return false;
    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "constant") == 0)
        return false;
    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "constant") == 0)
        return false;
    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "variable") == 0)
        return false;

    return false;
}

void
merge_terms(Term *arr[], int l, int m, int r)
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
merge_sort_terms(Term *arr[], int l, int r)
{
    if (l < r) {
        int m = l + (r - l)/2;

        merge_sort_terms(arr, l, m);
        merge_sort_terms(arr, m + 1, r);

        merge_terms(arr, l, m, r);
    }
}


bool
is_variable_term(Term* t)
{
    if(strcmp(t->meaning, "variable") == 0)
        return true;
    if(strcmp(t->meaning, "constant") == 0)
        return false;
    if(strcmp(t->meaning, "literal") == 0)
        return false;
    if(strcmp(t->meaning, "operator") == 0) {
        Operator *o = (Operator *) t->content;

        for (int i = 0;i < o->argc;i++)
            if (is_variable_term(o->argv[i]))
                return true;
       	return false;
    }
   	return false;
}

Term *
get_variable_terms_from_multiple(Term* t)
{
    if (strcmp(t->meaning, "variable") == 0)
        return copy_term(t);
    if (strcmp(t->meaning, "literal") == 0)
        return literal(1.0);
    if (strcmp(t->meaning, "constant") == 0)
        return literal(1.0);
    if (strcmp(t->meaning, "operator") == 0) {
        Operator *o = (Operator *) t->content;

        if (strcmp(o->name, "imaginary") == 0)
            return get_variable_terms_from_multiple(o->argv[0]);
        if (strcmp(o->name, "additive_inverse") == 0)
            return get_variable_terms_from_multiple(o->argv[0]);
        if (strcmp(o->name, "*") == 0) {
            Term *acc = NULL;

            int i;

            for (i = 0;i < o->argc;i++) {
                if (is_variable_term(o->argv[i])) {
                    acc = copy_term(o->argv[i]);
                    break;
                }
            }

            for (i++;i < o->argc;i++)
                if (is_variable_term(o->argv[i]))
                        acc = multiply(acc, copy_term(o->argv[i]));

            if (acc == NULL)
                return literal(1.0);
            else
                return acc;
        }
    }

    return literal(1.0); // don't know exactly what to do here
}

Term *
get_non_variable_terms_from_multiple(Term* t)
{
    if (strcmp(t->meaning, "variable") == 0)
        return literal(1.0);
    if (strcmp(t->meaning, "literal") == 0)
        return copy_term(t);
    if (strcmp(t->meaning, "constant") == 0)
        return copy_term(t);
    if (strcmp(t->meaning, "operator") == 0) {
        Operator *o = (Operator *) t->content;

        if (strcmp(o->name, "imaginary") == 0) {
            Term *t_1 = get_non_variable_terms_from_multiple(o->argv[0]);
            Term *t_2 = imaginary(t_1);
            free_term(t_1);
            return t_2;
        }
        if (strcmp(o->name, "additive_inverse") == 0) {
            Term *t_1 = get_non_variable_terms_from_multiple(o->argv[0]);
            Term *t_2 = additive_inverse(t_1);
            free_term(t_1);

            return additive_inverse(get_non_variable_terms_from_multiple(o->argv[0]));
        }
        if (strcmp(o->name, "*") == 0) {
            Term *acc = NULL;

            for (int i = 0;i < o->argc;i++) {
                if (!is_variable_term(o->argv[i])) {
                    if (acc == NULL)
                        acc = copy_term(o->argv[i]);
                    else
                        acc = multiply(acc, copy_term(o->argv[i]));
                }
            }

            if (acc == NULL)
                return literal(1.0);
            else
                return acc;
        }
    }

    return literal(0.0); // don't know exactly what to do here
}

Term *
combine_additive_variable_terms(Term* t)
{
    if (strcmp(t->meaning, "operator") != 0)
        return t;

    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "+") != 0)
        return t;

    int i, j;

    Term *non_variable, *variable;
    Term *current_non_variable, *current_variable;

    bool pair_found = false;

    for (i = 0;i < o->argc;i++) {
        non_variable = get_non_variable_terms_from_multiple(o->argv[i]);
        variable = get_variable_terms_from_multiple(o->argv[i]);

        for(j = i + 1;j < o->argc;j++) {
            current_non_variable = get_non_variable_terms_from_multiple(o->argv[j]);
            current_variable = get_variable_terms_from_multiple(o->argv[j]);

            if (is_equal(variable, current_variable) && is_variable_term(variable)) {
                pair_found = true;
                break;
            }
        }
        if (pair_found)
            break;

        free(non_variable);
        free(variable);
    }

    if (!pair_found)
        return t;
    Term *temp = multiply(add(copy_term(non_variable), copy_term(current_non_variable)), variable);

    Term *simple = temp;

    for (int k = 0;k < o->argc;k++) {
        if(k == i || k == j)
            continue;
        simple = add(simple, copy_term(o->argv[k]));
    }

    free_term(t);

    return simplify(simple);
}


int main()
{
// functions:
// c ... merge(term)

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

    Term *c = imaginary(add(x, a));
    Term *d = add(x, b);

    Term *e = multiply(c, d);

    print_term(e);
    printf("\n");

    print_term(simplify(e));
    printf("\n");


    return 0x00;
}
