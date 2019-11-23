#include <string.h>
#include "term.h"
#include "compare_term.h"

bool
is_less(Term *lhs, Term *rhs)
{
    // literal
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "literal") == 0) {
        Literal *lhs_literal = (Literal*) lhs->content;
        Literal *rhs_literal = (Literal*) rhs->content;

        if (lhs_literal->value < rhs_literal->value)
            return true;
        return false;
    }

    // constant
    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "constant") == 0) {
        Constant *lhs_constant = (Constant*) lhs->content;
        Constant *rhs_constant = (Constant*) rhs->content;

        if (strcmp(lhs_constant->name, rhs_constant->name) < 0)
            return true;
        return false;
    }

    // variable
    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "variable") == 0) {
        Variable *lhs_variable = (Variable*) lhs->content;
        Variable *rhs_variable = (Variable*) rhs->content;

        if (strcmp(lhs_variable->name, rhs_variable->name) < 0)
            return true;
        if (strcmp(lhs_variable->name, rhs_variable->name) > 0)
            return false;
        if (strcmp(lhs_variable->name, rhs_variable->name) == 0) {
            if (lhs_variable->indec < rhs_variable->indec)
                return true;
            if (lhs_variable->indec > rhs_variable->indec)
                return false;
            if (lhs_variable->indec == rhs_variable->indec) {
                for (int i = 0;i < lhs_variable->indec;i++) {
                    if (is_less(lhs_variable->index[i], rhs_variable->index[i]))
                        return true;
                    if (is_greater(lhs_variable->index[i], rhs_variable->index[i]))
                        return false;
                }
                return false;
            }
        }
    }

    // operator
    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "operator") == 0) {
        Operator *lhs_operator = (Operator*) lhs->content;
        Operator *rhs_operator = (Operator*) rhs->content;

        if (strcmp(lhs_operator->name, rhs_operator->name) < 0)
            return true;
        if (strcmp(lhs_operator->name, rhs_operator->name) > 0)
            return false;
        if (strcmp(lhs_operator->name, rhs_operator->name) == 0) {
            if (lhs_operator->argc < rhs_operator->argc)
                return true;
            if (lhs_operator->argc > rhs_operator->argc)
                return false;
            if (lhs_operator->argc == rhs_operator->argc) {
                for (int i = lhs_operator->argc - 1;i >= 0;i--) {
                    if (is_less(lhs_operator->argv[i], rhs_operator->argv[i]))
                        return true;
                    if (is_greater(lhs_operator->argv[i], rhs_operator->argv[i]))
                        return false;
                }
                return false;
            }
        }
    }

    // other
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

    return false;
}

bool
is_greater(Term *lhs, Term *rhs)
{
    // literal
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "literal") == 0) {
        Literal *lhs_literal = (Literal*) lhs->content;
        Literal *rhs_literal = (Literal*) rhs->content;

        if (lhs_literal->value > rhs_literal->value)
            return true;
        return false;
    }

    // constant
    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "constant") == 0) {
        Constant *lhs_constant = (Constant*) lhs->content;
        Constant *rhs_constant = (Constant*) rhs->content;

        if (strcmp(lhs_constant->name, rhs_constant->name) > 0)
            return true;
        return false;
    }

    // variable
    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "variable") == 0) {
        Variable *lhs_variable = (Variable*) lhs->content;
        Variable *rhs_variable = (Variable*) rhs->content;

        if (strcmp(lhs_variable->name, rhs_variable->name) > 0)
            return true;
        if (strcmp(lhs_variable->name, rhs_variable->name) < 0)
            return false;
        if (strcmp(lhs_variable->name, rhs_variable->name) == 0) {
            if (lhs_variable->indec > rhs_variable->indec)
                return true;
            if (lhs_variable->indec < rhs_variable->indec)
                return false;
            if (lhs_variable->indec == rhs_variable->indec) {
                for (int i = 0;i < lhs_variable->indec;i++) {
                    if (is_greater(lhs_variable->index[i], rhs_variable->index[i]))
                        return true;
                    if (is_less(lhs_variable->index[i], rhs_variable->index[i]))
                        return false;
                }
                return false;
            }
        }
        return false;
    }

    // operator
    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "operator") == 0) {
        Operator *lhs_operator = (Operator*) lhs->content;
        Operator *rhs_operator = (Operator*) rhs->content;

        if (strcmp(lhs_operator->name, rhs_operator->name) > 0)
            return true;
        if (strcmp(lhs_operator->name, rhs_operator->name) < 0)
            return false;
        if (strcmp(lhs_operator->name, rhs_operator->name) == 0) {
            if (lhs_operator->argc > rhs_operator->argc)
                return true;
            if (lhs_operator->argc < rhs_operator->argc)
                return false;
            if (lhs_operator->argc == rhs_operator->argc) {
                for (int i = lhs_operator->argc - 1;i >= 0;i--) {
                    if (is_greater(lhs_operator->argv[i], rhs_operator->argv[i]))
                        return true; 
                    if (is_less(lhs_operator->argv[i], rhs_operator->argv[i]))
                        return false;
                }
                return false;
            }
        }
        return false;
    }

    // other
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
    // literal
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "literal") == 0) {
        Literal *lhs_literal = (Literal*) lhs->content;
        Literal *rhs_literal = (Literal*) rhs->content;

        if (lhs_literal->value == rhs_literal->value)
            return true;
        return false;
    }

    // constant
    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "constant") == 0) {
        Constant *lhs_constant = (Constant*) lhs->content;
        Constant *rhs_constant = (Constant*) rhs->content;

        if (strcmp(lhs_constant->name, rhs_constant->name) == 0)
            return true;
        return false;
    }

    // variable
    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "variable") == 0) {
        Variable *lhs_variable = (Variable*) lhs->content;
        Variable *rhs_variable = (Variable*) rhs->content;

        if (strcmp(lhs_variable->name, rhs_variable->name) == 0) {
            if (lhs_variable->indec == rhs_variable->indec) {
                for (int i = 0;i < lhs_variable->indec;i++) {
                    if (is_greater(lhs_variable->index[i], rhs_variable->index[i]))
                        return false;
                    if (is_less(lhs_variable->index[i], rhs_variable->index[i]))
                        return false;
                }
                return true;
            }
        }
        return false;
    }

    // operator
    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "operator") == 0) {
        Operator *lhs_operator = (Operator*) lhs->content;
        Operator *rhs_operator = (Operator*) rhs->content;

        if (strcmp(lhs_operator->name, rhs_operator->name) == 0) {
            if (lhs_operator->argc == rhs_operator->argc) {
                for (int i = lhs_operator->argc - 1;i >= 0;i--) {
                    if (is_greater(lhs_operator->argv[i], rhs_operator->argv[i]))
                        return false;
                    if (is_less(lhs_operator->argv[i], rhs_operator->argv[i]))
                        return false;
                }
                return true;
            }
        }
        return false;
    }

    return false;
}
