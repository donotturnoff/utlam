#include "utlam.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s [input file]\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        error(FILE_ERR, "failed to open %s", argv[1]);
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *src = malloc((fsize + 1) * sizeof(char));
    fread(src, 1, fsize, f);
    fclose(f);

    src[fsize] = '\0';

    Term *t = parse(src);
    free(src);

    char *t_str = term_to_string(t);
    printf("%s -->> ", t_str);
    fflush(stdout);
    free(t_str);

    Term *result = eval(t);
    char *result_str = term_to_string(result);
    printf("%s\n", result_str);
    free(result_str);

    free_term(result);

    return 0;
}
