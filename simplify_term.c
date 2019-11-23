#include <string.h>
#include <stdlib.h>
#include "term.h"
#include "compare_term.h"
#include "sort_term.h"
#include "simplify_term.h"
#include "variable_term.h"

// TODO: simplify teilbare polynome like (x*x-1)=(x-1.0)*(x+1.0)*1/(x-1.0) => (x+1.0) (Polynomdivision)
// Term *simplify_polynom_division(Term *term);


// simplify_multiply_equal_variables (exponential)
// simplify differential
// simplify integral

Term *
simplify(Term *term)
{
    Operator *operator;
    Term *simple;

    if(strcmp(term->meaning, "operator") != 0)
        return term;

    operator = term->content;
    for(int i = 0;i < operator->argc;i++)
        operator->argv[i] = simplify(operator->argv[i]);

    simple = term;

    simple = simplify_double_imaginary(simple);
    simple = simplify_double_additive_inverse(simple);
    simple = simplify_double_multiple_inverse(simple);

    simple = swap_additive_inverse_imaginary(simple);

    simple = simplify_additive_inverse_zero(simple);
    simple = simplify_imaginary_zero(simple);

    simple = add_literals(simple);
    simple = add_imaginary_literals(simple);
    simple = simplify_additive_inverse_add(simple);
    simple = simplify_imaginary_add(simple);
    simple = simplify_added_zero(simple);
    simple = simplify_combine_added_neutral_terms(simple);
    simple = simplify_additive_assoziativity(simple);
    simple = simplify_order_added_terms(simple);

    simple = multiply_literals(simple);
    simple = simplify_multiple_assoziativity(simple);
    simple = simplify_multiple_additive_inverse(simple);
    simple = simplify_multiple_imaginary(simple);
    simple = simplify_multiple_inverse_imaginary(simple);
    simple = simplify_multiple_inverse_additive_inverse(simple);
    simple = simplify_multiplied_one(simple);
    simple = simplify_multiplied_zero(simple);
    simple = simplify_multiple_inverse_one(simple);
    simple = simplify_combine_multiplied_multiple_inverse(simple);
    simple = simplify_combine_multiplied_neutral_terms(simple);
    simple = simplify_order_multiplied_terms(simple);

    simple = simplify_fractured_literal(simple);
    simple = simplify_added_fractured_literal(simple);

    simple = simplify_with_distributive_law(simple);

    simple = simplify_recursive_multiple_inverse(simple);

    return simple;
}

Term *
simplify_double_imaginary(Term *term)
{
    Operator *operator_0, *operator_1;
    Term *simple;

    operator_0 = is_operator(term, "imaginary");
    if (operator_0 == NULL)
        return term;

    operator_1 = is_operator(operator_0->argv[0], "imaginary");
    if (operator_1 == NULL)
        return term;

    simple = additive_inverse(copy_term(operator_1->argv[0]));

    free_term(term);

    return simplify(simple);
}

Term *
simplify_double_additive_inverse(Term *term)
{
    Operator *operator_0, *operator_1;
    Term *simple;

    operator_0 = is_operator(term, "additive_inverse");
    if (operator_0 == NULL)
        return term;

    operator_1 = is_operator(operator_0->argv[0], "additive_inverse");
    if (operator_1 == NULL)
        return term;

    simple = copy_term(operator_1->argv[0]);

    free_term(term);

    return simplify(simple);
}

Term *
simplify_double_multiple_inverse(Term *term)
{
    Operator *operator_0, *operator_1;
    Term *simple;

    operator_0 = is_operator(term, "multiple_inverse");
    if (operator_0 == NULL)
        return term;

    operator_1 = is_operator(operator_0->argv[0], "multiple_inverse");
    if (operator_1 == NULL)
        return term;

    simple = copy_term(operator_1->argv[0]);

    free_term(term);

    return simplify(simple);
}

Term *swap_additive_inverse_imaginary(Term *term)
{
    Operator *operator_0, *operator_1;
    Term *simple;

    operator_0 = is_operator(term, "additive_inverse");
    if (operator_0 == NULL)
        return term;

    operator_1 = is_operator(operator_0->argv[0], "imaginary");
    if (operator_1 == NULL)
        return term;

    simple = imaginary(additive_inverse(copy_term(operator_1->argv[0])));

    free_term(term);

    return simplify(simple);
}

Term *
simplify_additive_inverse_zero(Term *term)
{
    Operator *operator;
    Literal *temp_literal;

    operator = is_operator(term, "additive_inverse");
    if (operator == NULL)
        return term;

    if (strcmp(operator->argv[0]->meaning, "literal") != 0)
        return term;

    temp_literal = operator->argv[0]->content;
    if (temp_literal->value != 0.0)
        return term;

    free_term(term);

    return literal(0.0);
}

Term *
simplify_imaginary_zero(Term * term)
{
    Operator *operator;
    Literal *temp_literal;

    operator = is_operator(term, "imaginary");
    if (operator == NULL)
        return term;

    if (strcmp(operator->argv[0]->meaning, "literal") != 0)
        return term;

    temp_literal = operator->argv[0]->content;
    if (temp_literal->value != 0.0)
        return term;

    free_term(term);

    return literal(0.0);
}

Term *
add_literals(Term *term)
{
    Operator *operator;
    Term *lhs_term, *rhs_term, *simple;

    int lhs_index, rhs_index;
    double lhs_value, rhs_value;

    operator = is_operator(term, "+");
    if (operator == NULL)
        return term;

    for (lhs_index = 0;lhs_index < operator->argc;lhs_index++) {
        lhs_term = operator->argv[lhs_index];

        if (strcmp(lhs_term->meaning, "literal") == 0) {
            Literal *temp_literal;

            temp_literal = lhs_term->content;
            lhs_value = temp_literal->value;
            break;
        }
        if (strcmp(lhs_term->meaning, "constant") == 0)
            continue;
        if (strcmp(lhs_term->meaning, "variable") == 0)
            continue;
        if (strcmp(lhs_term->meaning, "operator") == 0) {
            Operator *temp_operator;
            Term *temp_term;
            Literal *temp_literal;

            temp_operator = lhs_term->content;
            if (strcmp(temp_operator->name, "additive_inverse") != 0)
                continue;

            temp_term = temp_operator->argv[0];
            if (strcmp(temp_term->meaning, "literal") != 0)
                continue;

            temp_literal = temp_term->content;
            lhs_value = -(temp_literal->value);
            break;
        }
    }
    if(lhs_index >= operator->argc)
        return term;

    for (rhs_index = lhs_index + 1;rhs_index < operator->argc;rhs_index++) {
        if (lhs_index == rhs_index)
            continue;

        rhs_term = operator->argv[rhs_index];

        if (strcmp(rhs_term->meaning, "literal") == 0) {
            Literal *temp_literal;

            temp_literal = rhs_term->content;
            rhs_value = temp_literal->value;
            break;
        }
        if (strcmp(rhs_term->meaning, "constant") == 0)
            continue;
        if (strcmp(rhs_term->meaning, "variable") == 0)
            continue;
        if (strcmp(rhs_term->meaning, "operator") == 0) {
            Operator *temp_operator;
            Term *temp_term;
            Literal *temp_literal;

            temp_operator = rhs_term->content;
            if (strcmp(temp_operator->name, "additive_inverse") != 0)
                continue;

            temp_term = temp_operator->argv[0];
            if (strcmp(temp_term->meaning, "literal") != 0)
                continue;

            temp_literal = temp_term->content;
            rhs_value = -(temp_literal->value);
            break;
        }
    }
    if(rhs_index >= operator->argc)
        return term;

    simple = literal(lhs_value + rhs_value);

    for(int i = 0;i < operator->argc;i++) {
        if (i == lhs_index || i == rhs_index)
            continue;

        simple = add(simple, copy_term(operator->argv[i]));
    }

    free_term(term);

    return simplify(simple);
}

Term *
add_imaginary_literals(Term *term)
{
    Operator *operator, *imaginary_operator;
    Term *lhs_term, *rhs_term, *simple;

    int lhs_index, rhs_index;
    double lhs_value, rhs_value;

    operator = is_operator(term, "+");
    if (operator == NULL)
        return term;

    for (lhs_index = 0;lhs_index < operator->argc;lhs_index++) {
        imaginary_operator = is_operator(operator->argv[lhs_index], "imaginary");
        if (imaginary_operator == NULL)
            continue;

        lhs_term = imaginary_operator->argv[0];

        if (strcmp(lhs_term->meaning, "literal") == 0) {
            Literal *temp_literal;
            temp_literal = lhs_term->content;
            lhs_value = temp_literal->value;
            break;
        }
        if (strcmp(lhs_term->meaning, "constant") == 0)
            continue;
        if (strcmp(lhs_term->meaning, "variable") == 0)
            continue;
        if (strcmp(lhs_term->meaning, "operator") == 0) {
            Operator *temp_operator;
            Term *temp_term;
            Literal *temp_literal;

            temp_operator = lhs_term->content;
            if (strcmp(temp_operator->name, "additive_inverse") != 0)
                continue;

            temp_term = temp_operator->argv[0];
            if (strcmp(temp_term->meaning, "literal") != 0)
                continue;

            temp_literal = temp_term->content;
            lhs_value = -temp_literal->value;
            break;
        }
    }

    if(lhs_index >= operator->argc)
        return term;

    for (rhs_index = lhs_index + 1;rhs_index < operator->argc;rhs_index++) {
        if (lhs_index == rhs_index)
            continue;

        imaginary_operator = is_operator(operator->argv[rhs_index], "imaginary");
        if (imaginary_operator == NULL)
            continue;

        rhs_term = imaginary_operator->argv[0];

        if (strcmp(rhs_term->meaning, "literal") == 0) {
            Literal *temp_literal;

            temp_literal = rhs_term->content;
            rhs_value = temp_literal->value;
            break;
        }
        if (strcmp(rhs_term->meaning, "constant") == 0)
            continue;
        if (strcmp(rhs_term->meaning, "variable") == 0)
            continue;
        if (strcmp(rhs_term->meaning, "operator") == 0) {
            Operator *temp_operator;
            Term *temp_term;
            Literal *temp_literal;

            temp_operator = rhs_term->content;
            if (strcmp(temp_operator->name, "additive_inverse") != 0)
                continue;

            temp_term = temp_operator->argv[0];
            if (strcmp(temp_term->meaning, "literal") != 0)
                continue;

            temp_literal = temp_term->content;
            rhs_value = -temp_literal->value;
            break;
        }
    }

    if(rhs_index >= operator->argc)
        return term;

    simple = imaginary(literal(lhs_value + rhs_value));

    for(int i = 0;i < operator->argc;i++) {
        if (i == lhs_index || i == rhs_index)
            continue;

        simple = add(simple, copy_term(operator->argv[i]));
    }

    free_term(term);

    return simplify(simple);
}

Term *simplify_additive_inverse_add(Term *term)
{
    Operator *operator_0, *operator_1;
    Term *simple;

    operator_0 = is_operator(term, "additive_inverse");
    if (operator_0 == NULL)
        return term;

    operator_1 = is_operator(operator_0->argv[0], "+");
    if (operator_1 == NULL)
        return term;

    for (int i = 0;i < operator_1->argc;i++)
        operator_1->argv[i] = additive_inverse(operator_1->argv[i]);

    simple = copy_term(operator_0->argv[0]);

    free_term(term);

    return simplify(simple);
}

Term *simplify_imaginary_add(Term *term)
{
    Operator *operator_0, *operator_1;
    Term *simple;

    operator_0 = is_operator(term, "imaginary");
    if (operator_0 == NULL)
        return term;

    operator_1 = is_operator(operator_0->argv[0], "+");
    if (operator_1 == NULL)
        return term;

    for (int i = 0;i < operator_1->argc;i++)
        operator_1->argv[i] = imaginary(operator_1->argv[i]);

    simple = copy_term(operator_0->argv[0]);

    free_term(term);

    return simplify(simple);
}

Term *
simplify_added_zero(Term *term)
{
    Operator *operator;
    Literal *literal;
    Term *simple;
    int i, j;

    operator = is_operator(term, "+");
    if (operator == NULL)
        return term;

    for (i = 0;i < operator->argc;i++) {
        if (strcmp(operator->argv[i]->meaning, "literal") == 0)
            break;
    }
    if (i >= operator->argc)
        return term;

    literal = operator->argv[i]->content;
    if (literal->value != 0.0)
        return term;

    if (i == 0)
        j = 1;
    else
        j = 0;

    simple = copy_term(operator->argv[j]);

    for (int k = 0;k < operator->argc;k++) {
        if (i == k || j == k)
            continue;

        simple = add(simple, copy_term(operator->argv[k]));
    }

    free_term(term);

    return simplify(simple);
}

Term *
simplify_combine_added_neutral_terms(Term *term)
{
    Operator *operator_0, *operator_1;
    Term *term_1;
    int i, j;

    operator_0 = is_operator(term, "+");
    if (operator_0 == NULL)
        return term;

    for (i = 0;i < operator_0->argc;i++) {
        for (;i < operator_0->argc;i++) {
            operator_1 = is_operator(operator_0->argv[i], "additive_inverse");
            if (operator_1 == NULL)
                continue;

            term_1 = operator_1->argv[0];
            break;
        }
        if (i >= operator_0->argc)
            continue;

        for (j = 0;j < operator_0->argc;j++) {
            if (is_equal(operator_0->argv[j], term_1))
                break;
        }
        if (j >= operator_0->argc)
            continue;

        free_term(operator_0->argv[j]);
        free_term(operator_0->argv[i]);
        operator_0->argv[j] = literal(0.0);
        operator_0->argv[i] = literal(0.0);
    }
    if(i >= operator_0->argc)
        return term;

    return simplify(term);
}

Term *
simplify_additive_assoziativity(Term *term)
{
    Operator *operator_0, *operator_1;
    Term *simple;
    int i;

    operator_0 = is_operator(term, "+");
    if (operator_0 == NULL)
        return term;

    for(i = 0;i < operator_0->argc;i++) {
        operator_1 = is_operator(operator_0->argv[i], "+");
        if (operator_1 == NULL)
            continue;

        break;
    }
    if (i >= operator_0->argc)
        return term;

    simple = add(copy_term(operator_0->argv[0]), copy_term(operator_0->argv[1]));

    for (int i = 2;i < operator_0->argc;i++)
        simple = add(simple, copy_term(operator_0->argv[i]));

    free_term(term);

    return simplify(simple);
}

Term *simplify_order_added_terms(Term *term)
{
    Operator *operator;

    operator = is_operator(term, "+");
    if (operator == NULL)
        return term;

    merge_sort_terms(operator->argv, 0, operator->argc - 1);

    return term;
}

Term *
multiply_literals(Term *term)
{
    Operator *operator;
    Term *simple;

    operator = is_operator(term, "*");
    if (operator == NULL)
        return term;

    int lhs_index, rhs_index;
    Literal *lhs_literal, *rhs_literal;

    for (lhs_index = 0;lhs_index < operator->argc;lhs_index++) {
        if (strcmp(operator->argv[lhs_index]->meaning, "literal") != 0)
            continue;

        lhs_literal = operator->argv[lhs_index]->content;
        break;
    }

    if(lhs_index >= operator->argc)
        return term;

    for (rhs_index = 0;rhs_index < operator->argc;rhs_index++) {
        if (lhs_index == rhs_index)
            continue;

        if (strcmp(operator->argv[rhs_index]->meaning, "literal") != 0)
            continue;

        rhs_literal = operator->argv[rhs_index]->content;
        break;
    }

    if(rhs_index >= operator->argc)
        return term;

    simple = literal(lhs_literal->value * rhs_literal->value);

    for(int i = 0;i < operator->argc;i++) {
        if (i == lhs_index || i == rhs_index)
            continue;

        simple = multiply(simple, copy_term(operator->argv[i]));
    }

    free_term(term);

    return simplify(simple);
}

Term *
simplify_multiple_assoziativity(Term *term)
{
    Operator *operator;
    Term *simple;
    int i;

    operator = is_operator(term, "*");
    if (operator == NULL)
        return term;

    for(i = 0;i < operator->argc;i++) {
        Operator *o_1 = is_operator(operator->argv[i], "*");
        if (o_1 == NULL)
            continue;

        break;
    }
    if (i >= operator->argc)
        return term;

    simple = multiply(copy_term(operator->argv[0]), copy_term(operator->argv[1]));

    for (int i = 2;i < operator->argc;i++)
        simple = multiply(simple, copy_term(operator->argv[i]));

    free_term(term);

    return simplify(simple);
}

Term *
simplify_multiple_additive_inverse(Term *term)
{
    Operator *operator_0, *operator_1;
    Term *replacement;
    int i;

    operator_0 = is_operator(term, "*");
    if (operator_0 == NULL)
        return term;


    for(i = 0;i < operator_0->argc;i++) {
        operator_1 = is_operator(operator_0->argv[i], "additive_inverse");
        if (operator_1 == NULL)
            continue;

        replacement = copy_term(operator_1->argv[0]);
        break;
    }
    if (i >= operator_0->argc)
        return term;

    free_term(operator_0->argv[i]);
    operator_0->argv[i] = replacement;

    return simplify(additive_inverse(term));
}

Term *
simplify_multiple_imaginary(Term *term)
{
    Operator *operator_0, *operator_1;
    Term *replacement;
    int i;

    operator_0 = is_operator(term, "*");
    if (operator_0 == NULL)
        return term;

    for(i = 0;i < operator_0->argc;i++) {
        operator_1 = is_operator(operator_0->argv[i], "imaginary");
        if (operator_1 == NULL)
            continue;

        replacement = copy_term(operator_1->argv[0]);
        break;
    }
    if (i >= operator_0->argc)
        return term;

    free_term(operator_0->argv[i]);
    operator_0->argv[i] = replacement;

    return simplify(imaginary(term));
}

Term *
simplify_multiple_inverse_imaginary(Term *term)
{
    Operator *operator_0, *operator_1;
    Term *simple;

    operator_0 = is_operator(term, "multiple_inverse");
    if (operator_0 == NULL)
        return term;

    operator_1 = is_operator(operator_0->argv[0], "imaginary");
    if (operator_1 == NULL)
        return term;

    simple = imaginary(additive_inverse(multiple_inverse(copy_term(operator_1->argv[0]))));

    free_term(term);

    return simplify(simple);
}

Term *
simplify_multiple_inverse_additive_inverse(Term *term)
{
    Operator *operator_0, *operator_1;
    Term *simple;

    operator_0 = is_operator(term, "multiple_inverse");
    if (operator_0 == NULL)
        return term;

    operator_1 = is_operator(operator_0->argv[0], "additive_inverse");
    if (operator_1 == NULL)
        return term;

    simple = additive_inverse(multiple_inverse(copy_term(operator_1->argv[0])));

    free_term(term);

    return simplify(simple);
}

Term *
simplify_multiplied_one(Term *term)
{
    Operator *operator;
    Literal *literal;

    Term *simple;
    int i, j;

    operator = is_operator(term, "*");
    if (operator == NULL)
        return term;

    for (i = 0;i < operator->argc;i++) {
        if (strcmp(operator->argv[i]->meaning, "literal") == 0)
            break;
    }
    if (i >= operator->argc)
        return term;

    literal = operator->argv[i]->content;

    if (literal->value != 1.0)
        return term;

    if (i == 0)
        j = 1;
    else
        j = 0;

    simple = copy_term(operator->argv[j]);

    for (int k = 0;k < operator->argc;k++) {
        if (i == k || j == k)
            continue;

        simple = multiply(simple, copy_term(operator->argv[k]));
    }

    free_term(term);

    return simplify(simple);
}

Term *
simplify_multiplied_zero(Term *term)
{
    Operator *operator = is_operator(term, "*");
    if (operator == NULL)
        return term;

    int i;
    for (i = 0;i < operator->argc;i++) {
        if (strcmp(operator->argv[i]->meaning, "literal") != 0)
            continue;

        Literal *literal = operator->argv[i]->content;
        if (literal->value != 0.0)
            continue;

        break;
    }
    if (i >= operator->argc)
        return term;

    free_term(term);
    return literal(0.0);
}

Term *
simplify_multiple_inverse_one(Term *term)
{
    Operator *operator = is_operator(term, "multiple_inverse");
    if (operator == NULL)
        return term;

    if (strcmp(operator->argv[0]->meaning, "literal") != 0)
        return term;

    Literal *literal_0 = operator->argv[0]->content;
    if (literal_0->value != 1.0)
        return term;

    free_term(term);

    return literal(1.0);
}

Term *
simplify_combine_multiplied_multiple_inverse(Term *term)
{
    Operator *operator = is_operator(term, "*");
    if (operator == NULL)
        return term;

    int i, j, k;
    Term *term_1, *term_2;

    for (i = 0;i < operator->argc;i++) {
        Operator *operator_1 = is_operator(operator->argv[i], "multiple_inverse");
        if (operator_1 == NULL)
            continue;

        term_1 = operator_1->argv[0];
        j = i;
        break;
    }
    if (i >= operator->argc)
        return term;

    for (i++;i < operator->argc;i++) {
        Operator *operator_1 = is_operator(operator->argv[i], "multiple_inverse");
        if (operator_1 == NULL)
            continue;

        term_2 = operator_1->argv[0];
        k = i;
        break;
    }
    if (i >= operator->argc)
        return term;

    Term *simple = multiple_inverse(multiply(copy_term(term_1), copy_term(term_2)));

    for (i = 0;i < operator->argc;i++) {
        if (i == j || i == k)
            continue;

        simple = multiply(simple, copy_term(operator->argv[i]));
    }

    free_term(term);

    return simplify(simple);
}

Term *
simplify_combine_multiplied_neutral_terms(Term *term)
{
    Operator *operator = is_operator(term, "*");
    if (operator == NULL)
        return term;

    bool can_be_simplified = false;

    for (int i = 0;i < operator->argc;i++) {
        Operator *operator_1 = is_operator(operator->argv[i], "multiple_inverse");
        if (operator_1 == NULL)
            continue;

        Operator *operator_2 = is_operator(operator_1->argv[0], "*");
        if (operator_2 == NULL) {
            // no multiplication

            for (int k = 0;k < operator->argc;k++) {
                if (is_equal(operator->argv[k], operator_1->argv[0])) {
                    can_be_simplified = true;
                    free_term(operator->argv[k]);
                    free_term(operator_1->argv[0]);
                    operator->argv[k] = literal(1.0);
                    operator_1->argv[0] = literal(1.0);
                    break;
                }
            }
        } else {
            // yes multiplication
            for (int j = 0;j < operator_2->argc;j++) {
                for (int k = 0;k < operator->argc;k++) {
                    if (is_equal(operator->argv[k], operator_2->argv[j])) {
                        can_be_simplified = true;
                        free_term(operator->argv[k]);
                        free_term(operator_2->argv[j]);
                        operator->argv[k] = literal(1.0);
                        operator_2->argv[j] = literal(1.0);
                        break;
                    }
                }
            }
        }
    }

    if (!can_be_simplified)
        return term;
    else
        return simplify(term);
}

Term *simplify_order_multiplied_terms(Term *term)
{
    Operator *operator = is_operator(term, "*");
    if (operator == NULL)
        return term;

    merge_sort_terms(operator->argv, 0, operator->argc - 1);

    return term;
}

Term *
simplify_fractured_literal(Term *term)
{
    return term;
}

Term *
simplify_added_fractured_literal(Term *term)
{
    return term;
}

Term *
simplify_with_distributive_law(Term *term)
{
    Operator *operator_0 = is_operator(term, "*");
    if (operator_0 == NULL)
        return term;

    int i;
    Operator *temp_operator;

    bool is_variable = is_variable_term(term);

    for (i = 0;i < operator_0->argc;i++) {
        temp_operator = is_operator(operator_0->argv[i], "+");
        if (temp_operator == NULL)
            continue;

        if (is_variable)
            break;

        if (is_variable_term(operator_0->argv[i]))
            break;
        else
            continue;
    }
    if (i >= operator_0->argc)
        return term;

    Term **part = (Term **) malloc(sizeof(Term*) * temp_operator->argc);

    for (int j = 0;j < temp_operator->argc;j++)
        part[j] = copy_term(temp_operator->argv[j]);

    for (int j = 0;j < temp_operator->argc;j++) {
        for (int k = 0;k < operator_0->argc;k++) {
            if (i == k)
                continue;

            part[j] = multiply(part[j], copy_term(operator_0->argv[k]));
        }
    }


    Term *simple = operator("+", temp_operator->argc, part);

    free_term(term);

    return simple;
}

Term *
simplify_recursive_multiple_inverse(Term *term)
{
    Operator *operator = is_operator(term, "multiple_inverse");
    if (operator == NULL)
        return term;

    Operator *operator_1 = is_operator(operator->argv[0], "+");
    if (operator_1 == NULL)
        return term;

    Term *term_inverse;
    int i;
    for (i = 0;i < operator_1->argc;i++) {
        term_inverse = operator_1->argv[i];

        Operator *operator_2 = is_operator(term_inverse, "imaginary");
        if (operator_2 != NULL)
            term_inverse = operator_2->argv[0];

        Operator *operator_3 = is_operator(term_inverse, "additive_inverse");
        if (operator_3 != NULL)
            term_inverse = operator_3->argv[0];

        term_inverse = inside_of_multiple_inverse(term_inverse);
        if (term_inverse == NULL)
            continue;

        break;
    }
    if (i >= operator_1->argc)
        return term;

// TODO: testing because could throw error
    for(int i = 0;i < operator_1->argc;i++)
        operator_1->argv[i]
            = simplify(multiply(operator_1->argv[i], copy_term(term_inverse)));

    return simplify(multiply(copy_term(term_inverse), term));
}

Term *
inside_of_multiple_inverse(Term *term)
{
    Operator *operator = is_operator(term, "multiple_inverse");
    if (operator != NULL)
        return operator->argv[0];

    operator = is_operator(term, "*");
    if (operator == NULL)
        return NULL;

    Term *term_inverse;
    int i;

    for (i = 0;i < operator->argc;i++) {
        Operator *operator_1 = is_operator(operator->argv[i], "multiple_inverse");
        if (operator_1 == NULL)
            continue;

        term_inverse = operator_1->argv[0];
        break;
    }
    if (i >= operator->argc)
        return NULL;

    return term_inverse;
}



