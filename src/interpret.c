#include "utlam.h"
#include <stdlib.h>

void substitute(Term *t, Abs *in) {
    Term *v = in->vars;
    while (v) {
        Term *next = v->tc.var.next;
        Term *copy = copy_term(t, NULL);
        free(v->ns);
        free(v->name);
        v->type = copy->type;
        v->ns = copy->ns;
        v->name = copy->name;
        v->tc = copy->tc;
        free(copy);
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
            Term *result = tc.app.t1->tc.abs.body;
            free(tc.app.t1->tc.abs.arg);
            free(tc.app.t1);
            free_term(tc.app.t2);
            free(t);
            return result;
        } else {
            t->tc.app.t1 = reduce(tc.app.t1, altered);
            t->tc.app.t2 = reduce(tc.app.t2, altered);
            return t;
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
