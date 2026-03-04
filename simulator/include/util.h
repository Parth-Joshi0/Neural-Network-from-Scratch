#ifndef UTIL_H
#define UTIL_H
#include <stddef.h>

void *xalloc(size_t num, size_t size);
void create_transformation_matrix(float M[16], float l, float r, float b, float t);

#endif