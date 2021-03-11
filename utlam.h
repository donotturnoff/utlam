typedef enum term_type {
    VAR, ABS, APP
} TermType;

struct term;
struct env;

typedef struct var {
    char *name;
} Var;

typedef struct abs {
    char *arg;
    struct term *body;
} Abs;

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
} Term;

typedef struct env {
    char *name;
    Term *t;
    struct env *next;
} Env;

Term *var(char *name);
Term *abst(char *arg, Term *body);
Term *app(Term *t1, Term *t2);

void free_term(Term *t);

Term *eval(Term *t, Env *env);

char *term_to_string(Term *t);
