#include "utlam.h"
#include <stdlib.h>
#include <stdbool.h>

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

Term *reduce(Term *t, bool *altered) {
    TermType type = t->type;
    TermChoice tc = t->tc;
    Term *result;
    if (type == VAR) {
        result = t;
    } else if (type == ABS) {
        t->tc.abs.body = reduce(tc.abs.body, altered);
        result = t;
    } else {
        Term *t1 = tc.app.t1;
        Term *t2 = tc.app.t2;
        if (t1->type == ABS) { // Redex
            Abs abs = t1->tc.abs;
            substitute(t2, &abs);
            *altered = 1;
            result = abs.body;
            free(abs.arg);
            free(t1);
            free_term(t2);
            free(t);
        } else {
            t->tc.app.t1 = reduce(t1, altered);
            t->tc.app.t2 = reduce(t2, altered);
            result = t;
        }
    }
    return result;
}

Term *eval(Term *t) {
    bool altered;
    do {
        altered = 0;
        t = reduce(t, &altered);
    } while (altered);
    return t;
}
