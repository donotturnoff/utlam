#include "utlam.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>

void *malloc_or_die(size_t size) {
    void *mem = malloc(size);
    if (!mem && size) {
        fprintf(stderr, "malloc failed");
        abort();
    }
    return mem;
}

char *smprintf(char *fmt, ...) {
    if (!fmt) {
        return NULL;
    }
    va_list fmtargs;

    va_start(fmtargs, fmt);
    int len = vsnprintf(NULL, 0, fmt, fmtargs);
    va_end(fmtargs);

    char *ret = malloc_or_die(++len);

    va_start(fmtargs, fmt);
    vsnprintf(ret, len, fmt, fmtargs);
    va_end(fmtargs);

    return ret;
}

char *error_type_strings[3] = {"Lex", "Parse", "Evaluation"};

void error(ErrorType type, const char *format, ...) {
    va_list(args);
    fprintf(stderr, "%s error: ", error_type_strings[type]);
    va_start(args, format);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    exit(type+1); // Different error codes for different errors
}
