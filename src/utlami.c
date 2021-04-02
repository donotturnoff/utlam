#include "utlam.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <getopt.h>

void print_usage(char *prog) {
    fprintf(stderr, "Usage: %s [-d] file\n", prog);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    bool debug = 0;

    char opt;
    while ((opt = getopt(argc, argv, "d")) != -1) {
        switch (opt) {
            case 'd':
                debug = 1;
                break;
            default:
                print_usage(argv[0]);
        }
    }
    if (optind == argc) { // No file supplied
        print_usage(argv[0]);
    }

    FILE *f = fopen(argv[optind], "rb");
    if (!f) {
        error(FILE_ERR, "failed to open %s", argv[1]);
    }
    fseek(f, 0, SEEK_END);
    uint64_t fsize = ftell(f);
    rewind(f);

    char *src = malloc((fsize + 1) * sizeof(char));
    fread(src, 1, fsize, f);
    fclose(f);
    src[fsize] = '\0';
    if (debug) {
        printf("Source:\n%s\n", src);
    }

    Term *t = parse(src);
    free(src);

    if (debug) {
        char *t_str = term_to_string(t);
        printf("\nParsed:\n%s\n", t_str);
        free(t_str);
    }

    Term *result = eval(t);
    char *result_str = term_to_string(result);
    if (debug) {
        printf("\nResult:\n");
    }
    printf("%s\n", result_str);
    free(result_str);

    free_term(result);

    return 0;
}
