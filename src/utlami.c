#include "utlam.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    Term *t = app(abst("x", app(var("x"), var("x"))), abst("x", var("y")));

    char *t_str = term_to_string(t);
    printf("%s -->>", t_str);
    free(t_str);

    Term *result = eval(t, NULL);
    if (result) {
        char *result_str = term_to_string(result);
        printf(" %s\n", result_str);
        free(result_str);
    } else {
        printf("\n");
    }

    free_term(t);

    return 0;
}
