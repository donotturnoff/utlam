#include "utlam.h"
#include <stdlib.h>

Parser *parser(char *src) {
    Parser *p = malloc_or_die(sizeof(Parser));
    p->l = lexer(src);
    p->error = NULL;
    p->token = NULL;
    return p;
}

void free_parser(Parser *p) {
    if (p) {
        free(p->error);
        free_token(p->token);
        free(p->l);
        free(p);
    }
}

int accept(Parser *p, TokenType type) {
    if (!p->token) {
        p->error = smprintf("Unexpected character: %c", p->l->src[0]);
        return 0;
    }
    if (p->token->type == type) {
        free(p->token);
        p->token = next_token(p->l);
        return 1;
    }
    return 0;
}

int eat(Parser *p, TokenType type) {
    if (accept(p, type)) {
        return 1;
    } else {
        if (!p->error) {
            char *token_str = token_to_string(p->token);
            p->error = smprintf("Unexpected token: %s", token_str);
            free(token_str);
        }
        return 0;
    }
}

Term *parse_seq(Parser *p);
Term *parse_let(Parser *p);
void parse_export(Parser *p);
Term *parse_term(Parser *p);
Term *parse_app(Parser *p);
Term *parse_atom(Parser *p);

Term *parse(Parser *p) {
    p->token = next_token(p->l);
    return parse_seq(p);
}

Term *parse_seq(Parser *p) {
    Term *t = NULL;
    while (accept(p, EXPORT_TOK)) {
        parse_export(p);
    }
    if (p->error) {
        return NULL;
    }
    t = parse_term(p);
    if (p->error) {
        free_term(t);
        return NULL;
    }
    while (accept(p, EXPORT_TOK)) {
        parse_export(p);
    }
    if (p->error) {
        free_term(t);
        return NULL;
    }
    return t;
}

void parse_export(Parser *p) {

}

Term *parse_term(Parser *p) {
    if (accept(p, LAMBDA_TOK)) {
        char *arg = p->token->value;
        if (!eat(p, ID_TOK)) {
            free(arg);
            return NULL;
        }
        if (!eat(p, POINT_TOK)) {
            free(arg);
            return NULL;
        }
        Term *t = parse_term(p);
        if (!t) {
            free(arg);
            return NULL;
        }
        return abst(arg, t);
    } else if (accept(p, LET_TOK)) {
        return parse_let(p);
    } else {
        return parse_app(p);
    }
}

Term *parse_let(Parser *p) {
    char *arg = p->token->value;
    if (!eat(p, ID_TOK)) {
        free(arg);
        return NULL;
    }
    if (!eat(p, EQUALS_TOK)) {
        free(arg);
        return NULL;
    }
    Term *def = parse_term(p);
    if (!def) {
        free(arg);
        return NULL;
    }
    if (!eat(p, IN_TOK)) {
        free(arg);
        free_term(def);
        return NULL;
    }
    Term *body = parse_term(p);
    if (!body) {
        free(arg);
        free_term(def);
        return NULL;
    }
    return app(abst(arg, body), def);
}

Term *parse_app(Parser *p) {
    Term *t = parse_atom(p);
    if (!t) {
        return NULL;
    }
    if (!p->token) {
        free_term(t);
        return NULL;
    }
    TokenType type;
    while ((type = p->token->type) == LPAREN_TOK || type == ID_TOK) {
        Term *atom = parse_atom(p);
        if (!atom) {
            free_term(t);
            return NULL;
        }
        t = app(t, atom);
    }
    return t;
}

Term *parse_atom(Parser *p) {
    Term *t = NULL;
    char *name = p->token ? p->token->value : NULL;
    if (accept(p, LPAREN_TOK)) {
        t = parse_term(p);
        if (!eat(p, RPAREN_TOK)) {
            free_term(t);
            return NULL;
        }
    } else if (eat(p, ID_TOK)) {
        t = var(name);
    }
    return t;
}
