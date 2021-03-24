#include "utlam.h"
#include <stdlib.h>
#include <stdio.h>

void substitute(Term *t, Abs *in) {
    Term *v = in->vars;
    while (v) {
        Term *next = v->tc.var.next;
        Term *copy = copy_term(t, NULL);
        v->type = copy->type;
        v->ns = copy->ns;
        v->name = copy->name;
        v->tc = copy->tc;

        v = next;
    }
}

Term *reduce(Term *t, int *altered) {
    TermType type = t->type;
    TermChoice tc = t->tc;
    if (type == VAR) {
        return t;
    } else if (type == ABS) {
        t->tc.abs.body = reduce(tc.abs.body, altered);
        return t;
    } else {
        if (tc.app.t1->type == ABS) { // Redex
            substitute(tc.app.t2, &(tc.app.t1->tc.abs));
            *altered = 1;
            return tc.app.t1->tc.abs.body;
        } else {
            return app(reduce(tc.app.t1, altered), reduce(tc.app.t2, altered));
        }
    }
}

Term *eval(Term *t) {
    int altered;
    do {
        altered = 0;
        t = reduce(t, &altered);
    } while (altered);
    return t;
}
