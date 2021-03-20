#include "utlam.h"
#include <stdlib.h>

void bind(Abs *abs, Term *t) {
    abs->bound = t;
}

Term *eval_app(Term *t) {
    TermChoice tc = t->tc;
    Term *t1 = tc.app.t1;
    Term *t2 = tc.app.t2;
    Term *v2 = eval(t2);
    Term *v1 = eval(t1);
    if (v1->type != ABS) {
        return app(v1, v2);
        error(EVAL_ERR, "Can only apply abstractions");
    }
    bind(&(v1->tc.abs), v2);
    return eval(v1->tc.abs.body);
}

Term *eval(Term *t) {
    TermType type = t->type;
    TermChoice tc = t->tc;
    switch (type) {
        case VAR:
            return tc.var.binder && tc.var.binder->bound ? eval(tc.var.binder->bound) : t;
        case ABS:
            t->tc.abs.body = eval(tc.abs.body);
            return t;
        case APP:
            return eval_app(t);
        default:
            return NULL;
    }
}

