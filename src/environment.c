#include "utlam.h"
#include <stdlib.h>
#include <string.h>

Env *env_push(char *name, Abs *abs, Env *env) {
    Env *new_head = malloc_or_die(sizeof(Env));
    new_head->name = name;
    new_head->abs = abs;
    new_head->next = env;
    return new_head;
}

Env *env_pop(Env *env) {
    Env *next = env->next;
    free(env);
    return next;
}

Abs *env_get(char *name, Env *env) {
    while (env) {
        if (strcmp(name, env->name) == 0) {
            return env->abs;
        }
        env = env->next;
    }
    return NULL;
}

