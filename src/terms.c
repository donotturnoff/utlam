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
    if (binder) {
        t->tc.var.next = binder->vars;
        binder->vars = t;
    } else {
        t->tc.var.next = NULL;
    }
    return t;
}

Term *abst(char *arg, Term *body) {
    Term *t = malloc_or_die(sizeof(Term));
    t->type = ABS;
    t->ns = NULL;
    t->name = NULL;
    t->tc.abs.arg = arg;
    t->tc.abs.body = body;
    t->tc.abs.vars = NULL;
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

Term *copy_term(Term *t, Env *env) {
    TermType type = t->type;
    TermChoice tc = t->tc;
    if (type == VAR) {
        char *ns = smprintf(t->ns);
        char *name = smprintf(t->name);
        Abs *binder = env_get(name, env);
        return var(ns, name, binder);
    } else if (type == ABS) {
        char *arg = smprintf(tc.abs.arg);
        Term *abs = abst(arg, NULL);
        env = env_push(arg, &(abs->tc.abs), env);
        Term *body = copy_term(tc.abs.body, env);
        env = env_pop(env);
        abs->tc.abs.body = body;
        return abs;
    } else {
        return app(copy_term(tc.app.t1, env), copy_term(tc.app.t2, env));
    }
}

char *term_to_string(Term *t) {
    char *ns = t->ns;
    char *name = t->name;
    TermType type = t->type;
    TermChoice tc = t->tc;
    if (name) {
        if (ns) {
            return smprintf("%s::%s", ns, name);
        } else {
            return smprintf("%s", name);
        }
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

