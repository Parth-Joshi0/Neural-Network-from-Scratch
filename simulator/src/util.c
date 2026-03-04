#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void *xalloc(size_t num, size_t size) {
    void *ptr = calloc(num, size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void create_transformation_matrix(float M[16], float l, float r, float b, float t) {
    M[0]  =  2.0f / (r - l);
    M[1]  =  0.0f;
    M[2]  =  0.0f;
    M[3]  =  0.0f;

    M[4]  =  0.0f;
    M[5]  =  2.0f / (t - b);
    M[6]  =  0.0f;
    M[7]  =  0.0f;

    M[8]  =  0.0f;
    M[9]  =  0.0f;
    M[10] = -1.0f;
    M[11] =  0.0f;

    M[12] = -(r + l) / (r - l);
    M[13] = -(t + b) / (t - b);
    M[14] =  0.0f;
    M[15] =  1.0f;
}