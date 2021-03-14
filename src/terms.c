#include "utlam.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Term *var(char *name) {
    Term *t = malloc_or_die(sizeof(Term));
    t->type = VAR;
    t->tc.var.name = name;
    return t;
}

Term *abst(char *arg, Term *body) {
    Term *t = malloc_or_die(sizeof(Term));
    t->type = ABS;
    t->tc.abs.arg = arg;
    t->tc.abs.body = body;
    return t;
}

Term *app(Term *t1, Term *t2) {
    Term *t = malloc_or_die(sizeof(Term));
    t->type = APP;
    t->tc.app.t1 = t1;
    t->tc.app.t2 = t2;
    return t;
}

void free_term(Term *t) {
    if (t) {
        TermType type = t->type;
        TermChoice tc = t->tc;
        switch (type) {
            case VAR:
                free(tc.var.name);
                free(t);
                break;
            case ABS:
                free(tc.abs.arg);
                free_term(tc.abs.body);
                free(t);
                break;
            case APP:
                free_term(tc.app.t1);
                free_term(tc.app.t2);
                free(t);
                break;
            default:
                free(t);
        }
    }
}

char *term_to_string(Term *t) {
    if (!t) return NULL;
    TermType type = t->type;
    TermChoice tc = t->tc;
    if (type == VAR) {
        return smprintf(tc.var.name);
    } else if (type == ABS) {
        assert(tc.abs.body != NULL);
        assert(tc.abs.arg != NULL);
        char *body_str = term_to_string(tc.abs.body);
        char *str = smprintf("(\\%s.%s)", tc.abs.arg, body_str);
        free(body_str);
        return str;
    } else if (type == APP) {
        char *t1_str = term_to_string(tc.app.t1);
        char *t2_str = term_to_string(tc.app.t2);
        assert(tc.app.t1 != NULL);
        assert(tc.app.t2 != NULL);
        char *str = smprintf("%s %s", t1_str, t2_str);
        free(t1_str);
        free(t2_str);
        return str;
    } else {
        return NULL;
    }
}
