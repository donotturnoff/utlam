#include "utlam.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Term *var(char *ns, char *name, Abs *binder) {
    Term *t = malloc_or_die(sizeof(Term));
    t->type = VAR;
    t->ns = ns;
    t->name = name;
    t->tc.var.binder = binder;
    return t;
}

Term *abst(char *arg, Term *body) {
    Term *t = malloc_or_die(sizeof(Term));
    t->type = ABS;
    t->ns = NULL;
    t->name = NULL;
    t->tc.abs.arg = arg;
    t->tc.abs.bound = NULL;
    t->tc.abs.body = body;
    return t;
}

Term *app(Term *t1, Term *t2) {
    Term *t = malloc_or_die(sizeof(Term));
    t->type = APP;
    t->ns = NULL;
    t->name = NULL;
    t->tc.app.t1 = t1;
    t->tc.app.t2 = t2;
    return t;
}

void free_term(Term *t) {
    if (t) {
        TermType type = t->type;
        TermChoice tc = t->tc;
        free(t->ns);
        free(t->name);
        if (type == ABS) {
            free(tc.abs.arg);
            free_term(tc.abs.body);
        } else if (type == APP) {
            free_term(tc.app.t1);
            free_term(tc.app.t2);
        }
        free(t);
    }
}

char *term_to_string(Term *t) {
    char *ns = t->ns;
    char *name = t->name;
    if (name) {
        if (ns) {
            return smprintf("%s::%s", ns, name);
        } else {
            return smprintf("%s", name);
        }
    }
    TermType type = t->type;
    TermChoice tc = t->tc;
    if (type == VAR) {
        return term_to_string(tc.var.binder->bound);
    } else if (type == ABS) {
        char *body_str = term_to_string(tc.abs.body);
        char *str = smprintf("(\\%s.%s)", tc.abs.arg, body_str);
        free(body_str);
        return str;
    } else {
        char *t1_str = term_to_string(tc.app.t1);
        char *t2_str = term_to_string(tc.app.t2);
        char *str = smprintf("(%s %s)", t1_str, t2_str);
        free(t1_str);
        free(t2_str);
        return str;
    }
}
