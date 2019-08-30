#ifndef VARIABLE_TERMS_H
#define VARIABLE_TERMS_H

static bool is_variable_term(Term* t);
static Term *get_variable_terms_from_multiple(Term* t);
static Term *get_non_variable_terms_from_multiple(Term* t);


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
            return imaginary(get_non_variable_terms_from_multiple(o->argv[0]));
        }
        if (strcmp(o->name, "additive_inverse") == 0) {
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

    return literal(1.0); // don't know exactly what to do here
}

#endif /* VARIABLE_TERMS_H */
