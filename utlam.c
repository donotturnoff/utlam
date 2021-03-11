#include "lambda.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Term *var(char *name) {
    Term *t = malloc(sizeof(Term));
    t->type = VAR;
    t->tc.var.name = name;
    return t;
}

Term *abst(char *arg, Term *body) {
    Term *t = malloc(sizeof(Term));
    t->type = ABS;
    t->tc.abs.arg = arg;
    t->tc.abs.body = body;
    return t;
}

Term *app(Term *t1, Term *t2) {
    Term *t = malloc(sizeof(Term));
    t->type = APP;
    t->tc.app.t1 = t1;
    t->tc.app.t2 = t2;
    return t;
}

void free_term(Term *t) {
    TermType type = t->type;
    TermChoice tc = t->tc;
    switch (type) {
        case VAR:
            free(t);
            break;
        case ABS:
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

Env *env_push(char *name, Term *t, Env *env) {
    Env *new_head = malloc(sizeof(Env));
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
    if (v1->type != ABS) {
        fprintf(stderr, "Can only apply abstractions\n");
        return NULL;
    }
    Term *v2 = eval(t2, env);
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
            fprintf(stderr, "Unknown term type\n");
    }
    return result;
}

char *var_to_string(char *name) {
    char *str = malloc((strlen(name) + 1) * sizeof(char));
    sprintf(str, "%s", name);
    return str;
}

char *abs_to_string(char *arg, Term *body) {
    char *body_str = term_to_string(body);
    if (!body_str) return NULL;
    size_t len = 4 + strlen(arg) + strlen(body_str);
    char *str = malloc((len + 1) * sizeof(char));
    sprintf(str, "(\\%s.%s)", arg, body_str);
    free(body_str);
    return str;
}

char *app_to_string(Term *t1, Term *t2) {
    char *t1_str = term_to_string(t1);
    if (!t1_str) return NULL;
    char *t2_str = term_to_string(t2);
    if (!t2_str) return NULL;
    size_t len = 1 + strlen(t1_str) + strlen(t2_str);
    char *str = malloc((len + 1) * sizeof(char));
    sprintf(str, "%s %s", t1_str, t2_str);
    free(t1_str);
    free(t2_str);
    return str;
}

char *term_to_string(Term *t) {
    if (!t) return NULL;
    TermType type = t->type;
    TermChoice tc = t->tc;
    switch (type) {
        case VAR:   return var_to_string(tc.var.name);
        case ABS:   return abs_to_string(tc.abs.arg, tc.abs.body);
        case APP:   return app_to_string(tc.app.t1, tc.app.t2);
        default:    return NULL;
    }
}
