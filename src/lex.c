#include "utlam.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Lexer *lexer(char *src) {
    Lexer *l = malloc_or_die(sizeof(Lexer));
    l->src = src;
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
    while (l->src[0] != '\0' && is_whitespace(l->src[0])) {
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
    if (i == 2 && strncmp(src, "in", 2) == 0) {
        t = token(IN_TOK, NULL);
    } else if (i == 3 && strncmp(src, "let", 3) == 0) {
        t = token(LET_TOK, NULL);
    } else if (i == 6 && strncmp(src, "export", 6) == 0) {
        t = token(EXPORT_TOK, NULL);
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
            t = NULL;
        }
    } else if (c == '\0') {
        t = token(EOF_TOK, NULL);
    } else if (is_id_char(c)) {
        t = consume_id(l);
    } else {
        t = NULL;
    }

    if (t) {
        l->src++;
    }

    return t;
}

char *token_strings[11] = {"EOF", "\\", ".", "=", ";", "(", ")", "::", "export", "let", "in"};

char *token_to_string(Token *t) {
    if (!t) {
        return NULL;
    }
    TokenType type = t->type;
    if (type == ID_TOK) {
        return smprintf(t->value);
    } else if (EOF_TOK <= type && type <= IN_TOK) {
        return smprintf(token_strings[type]);
    } else {
        return NULL;
    }
}
