#include "utlam.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>

void *malloc_or_die(size_t size) {
    void *mem = malloc(size);
    if (!mem && size) {
        fprintf(stderr, "malloc failed\n");
        abort();
    }
    return mem;
}

char *smprintf(char *fmt, ...) {
    va_list fmtargs;
    char *ret;
    int len;

    va_start(fmtargs, fmt);
    len = vsnprintf(NULL, 0, fmt, fmtargs);
    va_end(fmtargs);

    ret = malloc_or_die(++len);

    va_start(fmtargs, fmt);
    vsnprintf(ret, len, fmt, fmtargs);
    va_end(fmtargs);

    return ret;
}

