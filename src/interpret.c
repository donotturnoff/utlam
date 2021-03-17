#include "utlam.h"
#include <stdlib.h>

void bind(Abs *abs, Term *t) {
    abs->bound = t;
}

Term *eval_app(Term *t1, Term *t2) {
    Term *v1 = eval(t1);
    if (v1->type != ABS) {
        error(EVAL_ERR, "Can only apply abstractions");
    }
    Term *v2 = eval(t2);
    bind(&(v1->tc.abs), v2);
    return eval(v1->tc.abs.body);
}

Term *eval(Term *t) {
    TermType type = t->type;
    TermChoice tc = t->tc;
    Term *result = NULL;
    switch (type) {
        case VAR:
            return tc.var.binder ? tc.var.binder->bound : t;
            break;
        case ABS:
            return t;
            break;
        case APP:
            return eval_app(tc.app.t1, tc.app.t2);
            break;
        default:
            error(EVAL_ERR, "Unknown term type");
    }
}

