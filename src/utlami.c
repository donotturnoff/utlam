#include "utlam.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s [source code]\n", argv[0]);
        return 1;
    }
    char *src = argv[1];

    Term *t = parse(src);

    char *t_str = term_to_string(t);
    printf("%s -->> ", t_str);
    fflush(stdout);
    free(t_str);

    Term *result = eval(t, NULL);
    if (result) {
        char *result_str = term_to_string(result);
        printf("%s\n", result_str);
        free(result_str);
    }

    free_term(t);

    return 0;
}
