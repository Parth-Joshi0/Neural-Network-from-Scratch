#ifndef RAY_CAST_H
#define RAY_CAST_H

#include "types.h"
#include "quad_tree.h"

typedef struct {
    Point point;
    float distance;
    int hit;
} RayHit;

RayHit cast_ray(QuadTreeNode* node, Point origin, float direction, float max_distance);

#endif