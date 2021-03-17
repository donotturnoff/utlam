#include <stddef.h>

typedef enum error_type {
	LEX_ERR, PARSE_ERR, EVAL_ERR, FILE_ERR
} ErrorType;

typedef enum term_type {
    VAR, ABS, APP
} TermType;

struct term;

typedef struct abs {
    char *arg;
	struct term *bound;
    struct term *body;
} Abs;

typedef struct var {
	Abs *binder;
    struct term **export;
} Var;

typedef struct app {
    struct term *t1, *t2;
} App;

typedef union term_choice {
    Var var;
    Abs abs;
    App app;
} TermChoice;

typedef struct term {
    TermType type;
    TermChoice tc;
    char *ns;
    char *name;
} Term;

typedef enum token_type {
    EOF_TOK, LAMBDA_TOK, POINT_TOK, EQUALS_TOK,
    SEMICOLON_TOK, LPAREN_TOK, RPAREN_TOK, NAMESPACE_TOK,
    EXPORT_TOK, LET_TOK, IN_TOK, ID_TOK
} TokenType;

typedef struct token {
    TokenType type;
    char *value;
} Token;

Term *var(char *ns, char *name, Abs *binder, Term **export);
Term *abst(char *arg, Term *body);
Term *app(Term *t1, Term *t2);
void free_term(Term *t);
Term *eval(Term *t);
void bind(Abs *abs, Term *t);
char *term_to_string(Term *t);

Token *next_token(char **src);
void free_token(Token *t);
char *token_to_string(Token *t);
char *token_type_to_string(TokenType type);

Term *parse(char *src);

void *malloc_or_die(size_t size);
char *smprintf(char *fmt, ...);
void error(ErrorType type, const char *format, ...);
