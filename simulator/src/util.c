#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void *xalloc(size_t num, size_t size) {
    void *ptr = calloc(num, size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}