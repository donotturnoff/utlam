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
    struct term *body;
	struct term *vars;
} Abs;

typedef struct var {
	Abs *binder;
    struct term *next;
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
    char *ns, *name;
    TermType type;
    TermChoice tc;
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

typedef struct env {
    char *name;
    Abs *abs;
    struct env *next;
} Env;

Term *var(char *ns, char *name, Abs *binder);
Term *abst(char *arg, Term *body);
Term *app(Term *t1, Term *t2);
void free_term(Term *t);
Term *copy_term(Term *t, Env *env);
char *term_to_string(Term *t);

Term *eval(Term *t);

Token *next_token(char **src);
void free_token(Token *t);
char *token_to_string(Token *t);
char *token_type_to_string(TokenType type);
Term *parse(char *src);

Env *env_push(char *name, Abs *abs, Env *env);
Env *env_pop(Env *env);
Abs *env_get(char *name, Env *env);

void *malloc_or_die(size_t size);
char *smprintf(char *fmt, ...);
void error(ErrorType type, const char *format, ...);
