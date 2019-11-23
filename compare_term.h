#ifndef COMPARE_TERMS_H
#define COMPARE_TERMS_H

static bool is_less(Term *lhs, Term *rhs);
static bool is_greater(Term *lhs, Term *rhs);
static bool is_equal(Term *lhs, Term *rhs);

bool
is_less(Term *lhs, Term *rhs)
{
    if (strcmp(lhs->meaning, "literal") == 0 &&
        strcmp(rhs->meaning, "literal") == 0) {
        Literal *lhs_l = (Literal*) lhs->content;
        Literal *rhs_l = (Literal*) rhs->content;

        if (lhs_l->value < rhs_l->value)
            return true;
        return false;
    }

    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "constant") == 0) {
        Constant *lhs_l = (Constant*) lhs->content;
        Constant *rhs_l = (Constant*) rhs->content;

        if (strcmp(lhs_l->name, rhs_l->name) < 0)
            return true;
        return false;
    }

    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "variable") == 0) {
        Variable *lhs_v = (Variable*) lhs->content;
        Variable *rhs_v = (Variable*) rhs->content;

        if (strcmp(lhs_v->name, rhs_v->name) < 0)
            return true;
        if (strcmp(lhs_v->name, rhs_v->name) > 0)
            return false;
        if (strcmp(lhs_v->name, rhs_v->name) == 0) {
            if (lhs_v->indec < rhs_v->indec)
                return true;
            if (lhs_v->indec > rhs_v->indec)
                return false;
            if (lhs_v->indec == rhs_v->indec) {
                for (int i = 0;i < lhs_v->indec;i++) {
                    if (is_less(lhs_v->index[i], rhs_v->index[i]))
                        return true;
                    if (is_greater(lhs_v->index[i], rhs_v->index[i]))
                        return false;
                }
                return false;
            }
        }
        return false;
    }

    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "operator") == 0) {
        Operator *lhs_o = (Operator*) lhs->content;
        Operator *rhs_o = (Operator*) rhs->content;

        if (strcmp(lhs_o->name, rhs_o->name) < 0)
            return true;
        if (strcmp(lhs_o->name, rhs_o->name) > 0)
            return false;
        if (strcmp(lhs_o->name, rhs_o->name) == 0) {
            if (lhs_o->argc < rhs_o->argc)
                return true;
            if (lhs_o->argc > rhs_o->argc)
                return false;
            if (lhs_o->argc == rhs_o->argc) {
                for (int i = lhs_o->argc - 1;i >= 0;i--) {
                    if (is_less(lhs_o->argv[i], rhs_o->argv[i]))
                        return true;
                    if (is_greater(lhs_o->argv[i], rhs_o->argv[i]))
                        return false;
                }
                return false;
            }
        }
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
        return false;
    }

    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "constant") == 0) {
        Constant *lhs_l = (Constant*) lhs->content;
        Constant *rhs_l = (Constant*) rhs->content;

        if (strcmp(lhs_l->name, rhs_l->name) > 0)
            return true;
        return false;
    }

    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "variable") == 0) {
        Variable *lhs_v = (Variable*) lhs->content;
        Variable *rhs_v = (Variable*) rhs->content;

        if (strcmp(lhs_v->name, rhs_v->name) > 0)
            return true;
        if (strcmp(lhs_v->name, rhs_v->name) < 0)
            return false;
        if (strcmp(lhs_v->name, rhs_v->name) == 0) {
            if (lhs_v->indec > rhs_v->indec)
                return true;
            if (lhs_v->indec < rhs_v->indec)
                return false;
            if (lhs_v->indec == rhs_v->indec) {
                for (int i = 0;i < lhs_v->indec;i++) {
                    if (is_greater(lhs_v->index[i], rhs_v->index[i]))
                        return true;
                    if (is_less(lhs_v->index[i], rhs_v->index[i]))
                        return false;
                }
                return false;
            }
        }
        return false;
    }

    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "operator") == 0) {
        Operator *lhs_o = (Operator*) lhs->content;
        Operator *rhs_o = (Operator*) rhs->content;

        if (strcmp(lhs_o->name, rhs_o->name) > 0)
            return true;
        if (strcmp(lhs_o->name, rhs_o->name) < 0)
            return false;
        if (strcmp(lhs_o->name, rhs_o->name) == 0) {
            if (lhs_o->argc > rhs_o->argc)
                return true;
            if (lhs_o->argc < rhs_o->argc)
                return false;
            if (lhs_o->argc == rhs_o->argc) {
                for (int i = lhs_o->argc - 1;i >= 0;i--) {
                    if (is_greater(lhs_o->argv[i], rhs_o->argv[i]))
                        return true;
                    if (is_less(lhs_o->argv[i], rhs_o->argv[i]))
                        return false;
                }
                return false;
            }
        }
        return false;
    }

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
        return false;
    }

    if (strcmp(lhs->meaning, "constant") == 0 &&
        strcmp(rhs->meaning, "constant") == 0) {
        Constant *lhs_l = (Constant*) lhs->content;
        Constant *rhs_l = (Constant*) rhs->content;

        if (strcmp(lhs_l->name, rhs_l->name) == 0)
            return true;
        return false;
    }

    if (strcmp(lhs->meaning, "variable") == 0 &&
        strcmp(rhs->meaning, "variable") == 0) {
        Variable *lhs_v = (Variable*) lhs->content;
        Variable *rhs_v = (Variable*) rhs->content;

        if (strcmp(lhs_v->name, rhs_v->name) == 0) {
            if (lhs_v->indec == rhs_v->indec) {
                for (int i = 0;i < lhs_v->indec;i++) {
                    if (is_greater(lhs_v->index[i], rhs_v->index[i]))
                        return false;
                    if (is_less(lhs_v->index[i], rhs_v->index[i]))
                        return false;
                }
                return true;
            }
            return false;
        }
        return false;
    }

    if (strcmp(lhs->meaning, "operator") == 0 &&
        strcmp(rhs->meaning, "operator") == 0) {
        Operator *lhs_o = (Operator*) lhs->content;
        Operator *rhs_o = (Operator*) rhs->content;

        if (strcmp(lhs_o->name, rhs_o->name) == 0) {
            if (lhs_o->argc == rhs_o->argc) {
                for (int i = lhs_o->argc - 1;i >= 0;i--) {
                    if (is_greater(lhs_o->argv[i], rhs_o->argv[i]))
                        return false;
                    if (is_less(lhs_o->argv[i], rhs_o->argv[i]))
                        return false;
                }
                return true;
            }
            return false;
        }
        return false;
    }

    return false;
}

#endif /* COMPARE_TERMS_H */
