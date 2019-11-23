#include <string.h>
#include "term.h"
#include "compare_term.h"

bool
is_variable_term(Term* term)
{
    if(strcmp(term->meaning, "literal") == 0)
        return false;
    if(strcmp(term->meaning, "constant") == 0)
        return false;
    if(strcmp(term->meaning, "variable") == 0)
        return true;
    if(strcmp(term->meaning, "operator") == 0) {
        Operator *operator = (Operator *) term->content;

        for (int i = 0;i < operator->argc;i++)
            if (is_variable_term(operator->argv[i]))
                return true;
        return false;
    }
    return false;
}

Term *
get_variable_term(Term* term)
{
    if (strcmp(term->meaning, "literal") == 0)
        return literal(1.0);
    if (strcmp(term->meaning, "constant") == 0)
        return literal(1.0);
    if (strcmp(term->meaning, "variable") == 0)
        return copy_term(term);
    if (strcmp(term->meaning, "operator") == 0) {
        Operator *operator = (Operator *) term->content;

        if (strcmp(operator->name, "imaginary") == 0)
            return get_variable_term(operator->argv[0]);
        if (strcmp(operator->name, "+") == 0) {
            for (int i = 0;i < operator->argc;i++) {
                if (is_variable_term(operator->argv[i]))
                    return copy_term(term);
            }
            return literal(1.0);
        }
        if (strcmp(operator->name, "additive_inverse") == 0)
            return get_variable_term(operator->argv[0]);
        if (strcmp(operator->name, "*") == 0) {
            Term *accumulated_term = NULL;
            int i;

            for (i = 0;i < operator->argc;i++) {
                if (is_variable_term(operator->argv[i])) {
                    accumulated_term = copy_term(operator->argv[i]);
                    break;
                }
            }

            for (i++;i < operator->argc;i++) {
                if (is_variable_term(operator->argv[i])) {
                    accumulated_term = multiply(accumulated_term, copy_term(operator->argv[i]));
                }
            }

            if (accumulated_term == NULL)
                return literal(1.0);
            else
                return accumulated_term;
        }
        if (strcmp(operator->name, "multiple_inverse") == 0) {
            Term *temp_term = get_variable_term(operator->argv[0]);

            if (is_equal(temp_term, literal(1.0)))
                return literal(1.0);
            else
                return multiple_inverse(copy_term(temp_term));
        }
    }

    return literal(1.0);
}

Term *
get_non_variable_term(Term* term)
{
    if (strcmp(term->meaning, "literal") == 0)
        return copy_term(term);
    if (strcmp(term->meaning, "constant") == 0)
        return copy_term(term);
    if (strcmp(term->meaning, "variable") == 0)
        return literal(1.0);
    if (strcmp(term->meaning, "operator") == 0) {
        Operator *operator = (Operator *) term->content;

        if (strcmp(operator->name, "imaginary") == 0) {
            return imaginary(get_non_variable_term(operator->argv[0]));
        }
        if (strcmp(operator->name, "+") == 0) {
            for (int i = 0;i < operator->argc;i++) {
                if (is_variable_term(operator->argv[i]))
                    return literal(1.0);
            }
            return copy_term(term);
        }
        if (strcmp(operator->name, "additive_inverse") == 0) {
            return additive_inverse(get_non_variable_term(operator->argv[0]));
        }
        if (strcmp(operator->name, "*") == 0) {
            Term *accumulated_term = NULL;
            int i;

            for (i = 0;i < operator->argc;i++) {
                if (!is_variable_term(operator->argv[i])) {
                    accumulated_term = copy_term(operator->argv[i]);
                    break;
                }
            }

            for (i++;i < operator->argc;i++) {
                if (!is_variable_term(operator->argv[i])) {
                    accumulated_term = multiply(accumulated_term, copy_term(operator->argv[i]));
                }
            }

            if (accumulated_term == NULL)
                return literal(1.0);
            else
                return accumulated_term;
        }
        if (strcmp(operator->name, "multiple_inverse") == 0) {
            Term *temp_term = get_non_variable_term(operator->argv[0]);

            if (is_equal(temp_term, literal(1.0)))
                return literal(1.0);
            else
                return multiple_inverse(copy_term(temp_term));
        }
    }

    return literal(1.0);
}

