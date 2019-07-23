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
static Term *operator(char *name, int argc, Term **argv);

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
    if (strcmp(o->name, "i") == 0)
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

    return operator("i", 1, argv);
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

    return simple;
}

Term *
simplify_subtraction(Term *t)
{
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
    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "i") != 0)
        return t;

    Term *t_1 = o->argv[0];
    if (strcmp(t_1->meaning, "operator") != 0)
        return t;

    Operator *o_1 = (Operator *) t_1->content;
    if (strcmp(o_1->name, "i") != 0)
        return t;

    Term *simple = additive_inverse(o_1->argv[0]);
    free_term(t);

    return simplify(simple);
}

Term *
simplify_multiple_negative(Term *t)
{
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
    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "*") != 0)
        return t;

    int i;
    Term *replacement = NULL;

    for(i = 0;i < o->argc;i++) {
        if (strcmp(o->argv[i]->meaning, "operator") != 0)
            continue;

        Operator *temp = (Operator *) o->argv[i]->content;
        if (strcmp(temp->name, "i") != 0)
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
    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "additive_inverse") != 0)
        return t;

    Term *t_1 = o->argv[0];
    if (strcmp(t_1->meaning, "operator") != 0)
        return t;

    Operator *o_1 = (Operator *) t_1->content;
    if (strcmp(o_1->name, "i") != 0)
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
    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "additive_inverse") != 0)
        return t;

    if (strcmp(o->argv[0]->meaning, "operator") != 0)
        return t;

    Operator *o_1 = (Operator *) o->argv[0]->content;
    if (strcmp(o_1->name, "+") != 0)
        return t;

    Term *simple = add(additive_inverse(o_1->argv[0]), additive_inverse(o_1->argv[1]));

    for (int i = 2;i < o->argc;i++)
        simple = add(simple, additive_inverse(o->argv[i]));

    free_term(t);

    return simplify(simple);
}

Term *
simplify_imaginary_add(Term *t)
{
    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "i") != 0)
        return t;

    if (strcmp(o->argv[0]->meaning, "operator") != 0)
        return t;

    Operator *o_1 = (Operator *) o->argv[0]->content;
    if (strcmp(o_1->name, "+") != 0)
        return t;

    Term *temp, *simple = add(imaginary(o_1->argv[0]), imaginary(o_1->argv[1]));

    for (int i = 2;i < o->argc;i++)
        simple = add(simple, imaginary(o->argv[i]));

    free_term(t);

    return simplify(simple);
}

Term *
simplify_with_distributive_law(Term *t)
{
    Operator *o = (Operator *) t->content;
    if (strcmp(o->name, "*") != 0)
        return t;

    int i;

    Operator *temp_o;

    for (i = 0;i < o->argc;i++) {
        if (strcmp(o->argv[i]->meaning, "operator") != 0)
            continue;

        temp_o = o->argv[i]->content;
        if (strcmp(temp_o->name, "+") != 0)
            continue;

        break;
    }

    if (i >= o->argc)
        return t;

    Term **part = (Term **) malloc(sizeof(Term *) * (temp_o->argc));

    for (int j = 0;j < temp_o->argc;j++)
        part[j] = copy_term(temp_o->argv[j]);

    for (int k = 0;k < temp_o->argc;k++) {
        for (int j = 0;j < o->argc;j++) {
            if (j == i)
                continue;
            part[k] = multiply(part[k], copy_term(o->argv[j]));
        }
    }

    Term *simple = add(part[0], part[1]);
    for (int j = 2;j < temp_o->argc;j++)
        simple = add(simple, part[j]);

    free(part);
    free_term(t);

    return simplify(simple);
}

Term *
add_literals(Literal *lhs, Literal *rhs)
{


    return NULL;
}

int main()
{
// functions:
// simplify(term)

// solve(term) // for differential equations
// solve(term, variable)
// minimize(term, variable, ...)
// maximize(term, variable, ...)
// series(term, variable, ...)
// print(term)
// pretty_print(term)
// plot(term, variable)
// plot(term, variable, variable)
// plot(term, variable, variable, variable)

// operators:
// i .......... imaginary ............. {term}
// negative ... negative .............. {term}
// + .......... add ................... {term, ...}
// - .......... subtract .............. {term, term}
// * .......... multiply .............. {term, ...}
// / .......... divide ................ {term, term}
// = .......... equal ................. {term, term}

    Term *e = constant("e", atof("2.71"), atof("2.72"));
    Term *x = variable("x");
    Term *zero = literal(atof("0.01"));
    Term *N = variable("N");

    Term *a = add(imaginary(imaginary(e)), additive_inverse(x));

    Term *b = additive_inverse(imaginary(subtract(zero, x)));

    Term *c = multiply(a, b);

    Term *y = variable("y");

    Term *f = equal(y, c);

    print_term(f);
    printf("\n");

    print_term(simplify(f));
    printf("\n");

    free_term(f);
    free_term(e);
    free_term(x);
    free_term(zero);
    free_term(N);
    free_term(a);
    free_term(b);
    free_term(c);
    free_term(y);

    return 0x00;
}
