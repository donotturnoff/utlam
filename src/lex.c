#include "utlam.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Lexer *lexer(char *src) {
    Lexer *l = malloc_or_die(sizeof(Lexer));
    l->src = src;
    l->error = NULL;
    return l;
}

int is_id_char(char c) {
    return ('A' <= c && c <= 'z') || ('0' <= c && c <= '9') || c == '_';
}

int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

Token *token(TokenType type, char *value) {
    Token *t = malloc_or_die(sizeof(Token));
    t->type = type;
    t->value = value;
    return t;
}

void free_token(Token *t) {
    if (t) {
        free(t->value);
        free(t);
    }
}

void consume_whitespace(Lexer *l) {
    while (is_whitespace(l->src[0])) {
        l->src++;
    }
}

Token *consume_id(Lexer *l) {
    char *src = l->src;
    size_t i = 0;

    while (is_id_char(src[i])) {
        i++;
    }

    Token *t;
    if (i == 2 && src[0] == 'i' && src[1] == 'n') { // in
        t = token(IN_TOK, NULL);
    } else if (i == 3 && src[0] == 'l' && src[1] == 'e' && src[2] == 't') { // let
        t = token(LET_TOK, NULL);
    } else {
        char *str = malloc_or_die((i + 1) * sizeof(char));
        memcpy(str, src, i);
        str[i] = '\0';
        t = token(ID_TOK, str);
    }

    l->src += i-1;
    return t;
}

Token *next_token(Lexer *l) {
    consume_whitespace(l);
    char c = l->src[0];
    Token *t;
    if (c == '.') {
        t = token(POINT_TOK, NULL);
    } else if (c == '\\') {
        t = token(LAMBDA_TOK, NULL);
    } else if (c == '=') {
        t = token(EQUALS_TOK, NULL);
    } else if (c == ';') {
        t = token(SEMICOLON_TOK, NULL);
    } else if (c == '(') {
        t = token(LPAREN_TOK, NULL);
    } else if (c == ')') {
        t = token(RPAREN_TOK, NULL);
    } else if (c == ':') {
        if ((++(l->src))[0] == ':') {
            t = token(NAMESPACE_TOK, NULL);
        } else {
            l->error = "Expected two colons for namespace operator";
            t = NULL;
        }
    } else if (c == '\0') {
        t = token(EOF_TOK, NULL);
    } else if (is_id_char(c)) {
        t = consume_id(l);
    } else {
        l->error = "Illegal character";
        t = NULL;
    }

    if (t) {
        l->src++;
    }

    return t;
}
