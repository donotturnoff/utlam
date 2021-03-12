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

Token *consume_include(Lexer *l) {
    char *src = ++l->src;
    if (src[0] != '"') {
        l->error = "Expected string literal after #";
        return NULL;
    }
    src++;
    size_t i = 0;
    do {
        if (src[i] == '\0' || src[i] == '\r' || src[i] == '\n') {
            l->error = "Unterminated string literal in include";
            return NULL;
        }
        i++;
    } while (src[i] != '"');

    char *str = malloc_or_die((i + 1) * sizeof(char));
    memcpy(str, src, i);
    str[i] = '\0';

    l->src += i+1; // +1 to skip over initial "
    return token(INCLUDE_TOK, str);
}

Token *consume_id(Lexer *l) {
    char *src = l->src;
    size_t i = 0;

    while (is_id_char(src[i])) {
        i++;
    }

    Token *t;
    if (i == 3 && src[0] == 'l' && src[1] == 'e' && src[2] == 't') {
        t = token(LET_TOK, NULL); // If string is "let", produce LET_TOK
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
    switch (c) {
        case '\0':
            t = token(EOF_TOK, NULL);
            break;
        case '\\':
            t = token(LAMBDA_TOK, NULL);
            break;
        case '.':
            t = token(POINT_TOK, NULL);
            break;
        case '=':
            t = token(EQUALS_TOK, NULL);
            break;
        case ';':
            t = token(SEMICOLON_TOK, NULL);
            break;
        case '(':
            t = token(LPAREN_TOK, NULL);
            break;
        case ')':
            t = token(RPAREN_TOK, NULL);
            break;
        case '#':
            t = consume_include(l);
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            t = consume_id(l);
            break;
        default:
            l->error = "Illegal character";
            t = NULL;
    }
    if (t) {
        l->src++;
    }
    return t;
}
