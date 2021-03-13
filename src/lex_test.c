#include "utlam.h"
#include <stdio.h>
#include <stdlib.h>

void print_token(Token *t) {
    TokenType type = t->type;
    switch (type) {
        case EOF_TOK:
            printf("EOF\n");
            break;
        case LAMBDA_TOK:
            printf("LAMBDA\n");
            break;
        case POINT_TOK:
            printf("POINT\n");
            break;
        case EQUALS_TOK:
            printf("EQUALS\n");
            break;
        case SEMICOLON_TOK:
            printf("SEMICOLON\n");
            break;
        case LPAREN_TOK:
            printf("LPAREN\n");
            break;
        case RPAREN_TOK:
            printf("RPAREN\n");
            break;
        case LET_TOK:
            printf("LET\n");
            break;
        case IN_TOK:
            printf("IN\n");
            break;
        case NAMESPACE_TOK:
            printf("NAMESPACE\n");
            break;
        case ID_TOK:
            printf("ID(%s)\n", t->value);
            break;
    }
}

int main(void) {
    char *src = "let f = \\x.(x x) in Utils::g (\\y . (f y))";
    Lexer *l = lexer(src);
    Token *t;
    while ((t = next_token(l)) && t->type != EOF_TOK) {
        print_token(t);
        free_token(t);
    }
    if (!t) {
        printf("%s on input %c\n", l->error, l->src[0]);
    }
    free_token(t);
    free(l);
    return 0;
}
