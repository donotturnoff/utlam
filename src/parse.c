#include "utlam.h"
#include <stdlib.h>

typedef struct parser {
    char *src;
    Token *token;
} Parser;

void parse_export(Parser *p);
Term *parse_term(Parser *p);

Parser *parser(char *src) {
    Parser *p = malloc_or_die(sizeof(Parser));
    p->src = src;
    p->token = NULL;
    return p;
}

void free_parser(Parser *p) {
    if (p) {
        free_token(p->token);
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

int eat(Parser *p, TokenType type) {
    if (accept(p, type)) {
        return 1;
    } else {
        char *expected_str = token_type_to_string(type);
        char *token_str = token_to_string(p->token);
        char *msg = smprintf("expected %s, got %s", expected_str, token_str);
        free(token_str);
        free(expected_str);
        free_parser(p);
        error(PARSE_ERR, msg);
        return 0;
    }
}

Term *parse_atom(Parser *p) {
    Term *t = NULL;
    char *name = smprintf(p->token->value);
    if (accept(p, LPAREN_TOK)) {
        t = parse_term(p);
        eat(p, RPAREN_TOK);
    } else {
        eat(p, ID_TOK);
        t = var(name);
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
        Term *t = parse_term(p);
        return abst(arg, t);
    } else if (accept(p, LET_TOK)) {
        char *arg = smprintf(p->token->value);
        eat(p, ID_TOK);
        eat(p, EQUALS_TOK);
        Term *def = parse_term(p);
        eat(p, IN_TOK);
        Term *body = parse_term(p);
        return app(abst(arg, body), def);
    } else {
        return parse_app(p);
    }
}

void parse_export(Parser *p) {

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

