#ifndef SIMPLIFY_TERM_H_
#define SIMPLIFY_TERM_H_

Term *simplify(Term *term);

Term *simplify_double_imaginary(Term *term);
Term *simplify_double_additive_inverse(Term *term);
Term *simplify_double_multiple_inverse(Term *term);

Term *swap_additive_inverse_imaginary(Term *term);

Term *simplify_additive_inverse_zero(Term *term);
Term *simplify_imaginary_zero(Term *term);

Term *add_literals(Term *term);
Term *add_imaginary_literals(Term *term);
Term *simplify_additive_inverse_add(Term *term);
Term *simplify_imaginary_add(Term *term);
Term *simplify_added_zero(Term *term);
Term *simplify_combine_added_neutral_terms(Term *term);
Term *simplify_additive_assoziativity(Term *term);
Term *simplify_order_added_terms(Term *term);

Term *multiply_literals(Term *term);
Term *simplify_multiple_assoziativity(Term *term);
Term *simplify_multiple_additive_inverse(Term *term);
Term *simplify_multiple_imaginary(Term *term);
Term *simplify_multiple_inverse_imaginary(Term *term);
Term *simplify_multiple_inverse_additive_inverse(Term *term);
Term *simplify_multiplied_one(Term *term);
Term *simplify_multiplied_zero(Term *term);
Term *simplify_multiple_inverse_one(Term *term);
Term *simplify_combine_multiplied_multiple_inverse(Term *term);
Term *simplify_combine_multiplied_neutral_terms(Term *term);
Term *simplify_order_multiplied_terms(Term *term);

Term *simplify_fractured_literal(Term *term);
Term *simplify_added_fractured_literal(Term *term);

Term *simplify_with_distributive_law(Term *term);

Term *simplify_recursive_multiple_inverse(Term *term);
Term *inside_of_multiple_inverse(Term *t);


#endif // SIMPLIFY_TERM_H_
