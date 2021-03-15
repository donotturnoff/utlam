#include "utlam.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Env *env_push(char *name, Term *t, Env *env) {
    Env *new_head = malloc_or_die(sizeof(Env));
    new_head->name = name;
    new_head->t = t;
    new_head->next = env;
    return new_head;
}

Env *env_pop(Env *env) {
    Env *next = env->next;
    free(env);
    return next;
}

Term *env_get(char *name, Env *env) {
    while (env) {
        if (strcmp(name, env->name) == 0)
            return env->t;
        env = env->next;
    }
    return NULL;
}

Term *eval_app(Term *t1, Term *t2, Env *env) {
    Term *v1 = eval(t1, env);
    if (!v1) {
        return NULL;
    }
    if (v1->type != ABS) {
        error(EVAL_ERR, "Can only apply abstractions");
        return NULL;
    }
    Term *v2 = eval(t2, env);
    if (!v2) {
        return NULL;
    }
    env = env_push(v1->tc.abs.arg, v2, env);
    Term *result = eval(v1->tc.abs.body, env);
    env = env_pop(env);
    return result;
}

Term *eval(Term *t, Env *env) {
    TermType type = t->type;
    TermChoice tc = t->tc;
    Term *result = NULL;
    switch (type) {
        case VAR:
            result = env_get(tc.var.name, env);
            result = result ? result : t;
            break;
        case ABS:
            result = t;
            break;
        case APP:
            result = eval_app(tc.app.t1, tc.app.t2, env);
            break;
        default:
            result = NULL;
            error(EVAL_ERR, "Unknown term type");
    }
    return result;
}

