#include <stdio.h>
#include "term.h"
#include "simplify_term.h"

// 1. TODO: polinomial factoring
// 2. TODO: Integral
// 3. TODO: Differential

// 4. TODO: solve for variable
// 4. TODO: solve for all variables (seperatly)

// 5. TODO: plot 2d/3d



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
// ^ .......... power ................. {term, term}
// D .......... differential .......... {term, term} (term, variable)
// I .......... integral .............. {term, term [, term, term]} (term, variable [, term, term])
// = .......... equal ................. {term, term}

int main()
{
    Term *l = literal(3.0);
    Term *x = variable("x");

    print_term(l);
    printf("\n");

    print_term(x);
    printf("\n");

    Term *t = multiply(l, x);

    print_term(simplify(t));
    printf("\n");
    return 0x00;
}
