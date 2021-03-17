#include "utlam.h"
#include <stdlib.h>
#include <string.h>

// TODO: consistent naming between envs and exports

typedef struct env {
    char *name;
    Abs *abs;
    struct env *next;
} Env; // TODO: use hash table or something instead

typedef struct exports {
    char *ns, *name;
    Term **t;
    struct exports *next;
} Exports; // TODO: use hash table

typedef struct parser {
    char *src;
    Token *token;
    Env *env;
    Exports *exps;
} Parser;

void parse_export(Parser *p);
Term *parse_term(Parser *p);

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

Term **get_export(char *ns, char *name, Exports *exps) {
    while (exps) {
        if (strcmp(ns, exps->ns) == 0 && strcmp(name, exps->name) == 0) {
            return exps->t;
        }
        exps = exps->next;
    }
    return NULL;
}

Exports *add_export(char *ns, char *name, Term *t, Exports *exps) {
    Exports *cur = exps;
    while (cur) {
        if (strcmp(cur->ns, ns) == 0 && strcmp(cur->name, name) == 0) {
            if (*(cur->t)) { // Export already defind
                error(EVAL_ERR, "multiply-defined export %s::%s", ns, name);
            } else { // Export provisionally declared by its use, now fill it in
                *(cur->t) = t;
                return exps;
            }
        }
        cur = cur->next;
    }
    // Export not defined nor provisionally declared by use: create new export
    Exports *new_head = malloc_or_die(sizeof(Exports));
    new_head->ns = ns;
    new_head->name = name;
    new_head->t = malloc_or_die(sizeof(Term *));
    *(new_head->t) = t;
    new_head->next = exps;
    return new_head;
}

void free_exports(Exports *exps) {
    while (exps) {
        Exports *next = exps->next;
        free(exps);
        exps = next;
    }
}

Parser *parser(char *src) {
    Parser *p = malloc_or_die(sizeof(Parser));
    p->src = src;
    p->token = NULL;
    p->env = NULL;
    p->exps = NULL;
    return p;
}

void free_parser(Parser *p) {
    if (p) {
        free_token(p->token);
        free_exports(p->exps);
        free(p);
    }
}

int accept(Parser *p, TokenType type) {
    if (p->token->type == type) {
        free_token(p->token);
        p->token = next_token(&(p->src));
        return 1;
    }
    return 0;
}

void eat(Parser *p, TokenType type) {
    if (!accept(p, type)) {
        char *expected_str = token_type_to_string(type);
        char *token_str = token_to_string(p->token);
        char *msg = smprintf("expected %s, got %s", expected_str, token_str);
        free(token_str);
        free(expected_str);
        free_parser(p);
        error(PARSE_ERR, msg);
    }
}

Term *parse_atom(Parser *p) {
    Term *t = NULL;
    if (accept(p, LPAREN_TOK)) {
        t = parse_term(p);
        eat(p, RPAREN_TOK);
    } else {
        char *ns = smprintf(p->token->value);
        eat(p, ID_TOK);
        if (accept(p, NAMESPACE_TOK)) {
            char *name = smprintf(p->token->value);
            eat(p, ID_TOK);
            Term **export = get_export(ns, name, p->exps);
            if (!export) {
                // Provisionally add export to be filled in by definition later on
                p->exps = add_export(ns, name, NULL, p->exps);
                t = var(ns, name, NULL, p->exps->t);
            } else {
                t = var(ns, name, NULL, export);
            }
        } else {
            Abs *binder = env_get(ns, p->env); // ns is actually name here, not namespace
            if (binder) {
                t = var(NULL, NULL, binder, NULL);
            } else {
                t = var(NULL, ns, NULL, NULL);
            }
        }
    }
    return t;
}

Term *parse_app(Parser *p) {
    Term *t = parse_atom(p);
    TokenType type;
    while ((type = p->token->type) == LPAREN_TOK || type == ID_TOK) {
        Term *atom = parse_atom(p);
        t = app(t, atom);
    }
    return t;
}

Term *parse_term(Parser *p) {
    if (accept(p, LAMBDA_TOK)) {
        char *arg = smprintf(p->token->value);
        eat(p, ID_TOK);
        eat(p, POINT_TOK);
        Term *abs = abst(arg, NULL);
        p->env = env_push(arg, &(abs->tc.abs), p->env);
        Term *body = parse_term(p);
        p->env = env_pop(p->env);
        abs->tc.abs.body = body;
        return abs;
    } else if (accept(p, LET_TOK)) {
        char *arg = smprintf(p->token->value);
        eat(p, ID_TOK);
        eat(p, EQUALS_TOK);
        Term *def = parse_term(p);
        eat(p, IN_TOK);

        Term *abs = abst(arg, NULL);
        p->env = env_push(arg, &(abs->tc.abs), p->env);
        Term *body = parse_term(p);
        p->env = env_pop(p->env);
        abs->tc.abs.body = body;

        return app(abs, def); // TODO: investigate binding at compile time
    } else {
        return parse_app(p);
    }
}

void parse_export(Parser *p) {
    char *ns = smprintf(p->token->value);
    eat(p, ID_TOK);
    eat(p, NAMESPACE_TOK);
    char *name = smprintf(p->token->value);
    eat(p, ID_TOK);
    eat(p, EQUALS_TOK);
    Term *t = parse_term(p);
    t->ns = ns;
    t->name = name;
    eat(p, SEMICOLON_TOK);
    Term **export = get_export(ns, name, p->exps);
    if (export) {
        if (*export) { // Exported term already defined: error
        } else { // Exported term provisionally added by its use: needs filling in
            *export = t;
        }
    } else {
        p->exps = add_export(ns, name, t, p->exps);
    }
}

Term *parse_seq(Parser *p) {
    Term *t = NULL;
    while (accept(p, EXPORT_TOK)) {
        parse_export(p);
    }
    t = parse_term(p);
    while (accept(p, EXPORT_TOK)) {
        parse_export(p);
    }
    eat(p, EOF_TOK);
    return t;
}

Term *parse(char *src) {
    Parser *p = parser(src);
    p->token = next_token(&(p->src));
    Term *t = parse_seq(p);
    free_parser(p);
    return t;
}

