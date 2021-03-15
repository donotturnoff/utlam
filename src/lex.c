#include "utlam.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

void consume_whitespace(char **src) {
    char c = (*src)[0];
    while (c != '\0' && is_whitespace(c)) {
        c = (++(*src))[0];
    }
}

Token *consume_id(char **src) {
    char *str = *src;
    size_t i = 0;

    while (is_id_char(str[i])) {
        i++;
    }

    Token *t;
    if (i == 2 && strncmp(str, "in", 2) == 0) {
        t = token(IN_TOK, NULL);
    } else if (i == 3 && strncmp(str, "let", 3) == 0) {
        t = token(LET_TOK, NULL);
    } else if (i == 6 && strncmp(str, "export", 6) == 0) {
        t = token(EXPORT_TOK, NULL);
    } else {
        char *id = malloc_or_die((i + 1) * sizeof(char));
        memcpy(id, str, i);
        id[i] = '\0';
        t = token(ID_TOK, id);
    }

    (*src) += i-1;
    return t;
}

Token *next_token(char **src) {
    consume_whitespace(src);
    char c = (*src)[0];
    Token *t = NULL;
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
        if ((++(*src))[0] == ':') {
            t = token(NAMESPACE_TOK, NULL);
        }
    } else if (c == '\0') {
        t = token(EOF_TOK, NULL);
    } else if (is_id_char(c)) {
        t = consume_id(src);
    }

    if (t) {
        (*src)++;
    } else {
        error(LEX_ERR, "unexpected character %c", c);
    }

    return t;
}

char *token_strings[12] = {"EOF", "\\", ".", "=", ";", "(", ")", "::", "export", "let", "in", "identifier"};

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

char *token_type_to_string(TokenType type) {
    if (EOF_TOK <= type && type <= ID_TOK) {
        return smprintf(token_strings[type]);
    } else {
        return NULL;
    }
}
