#include "utlam.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

void *malloc_or_die(size_t size) {
    void *mem = malloc(size);
    if (!mem && size) {
        fprintf(stderr, "malloc failed\n");
        abort();
    }
    return mem;
}
