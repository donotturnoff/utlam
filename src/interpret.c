#include "utlam.h"
#include <stdlib.h>
#include <stdio.h>

void bind(Abs *abs, Term *t) {
    abs->bound = t;
}

Term *eval_app(Term *t1, Term *t2) {
    Term *v1 = eval(t1);
    if (!v1) {
        return NULL;
    }
    if (v1->type != ABS) {
        error(EVAL_ERR, "Can only apply abstractions");
        return NULL;
    }
    Term *v2 = eval(t2);
    if (!v2) {
        return NULL;
    }
    bind(&(v1->tc.abs), v2);
    Term *result = eval(v1->tc.abs.body);
    return result;
}

Term *eval(Term *t) {
    TermType type = t->type;
    TermChoice tc = t->tc;
    Term *result = NULL;
    switch (type) {
        case VAR:
            result = tc.var.binder ? tc.var.binder->bound : t;
            break;
        case ABS:
            result = t;
            break;
        case APP:
            result = eval_app(tc.app.t1, tc.app.t2);
            break;
        default:
            result = NULL;
            error(EVAL_ERR, "Unknown term type");
    }
    return result;
}

