#ifndef QUADTREE_H
#define QUADTREE_H

#include "types.h"
#include "track_internals.h"

typedef struct {
    int min_x;
    int min_y;
    int max_x;
    int max_y;
} Bounds;

typedef struct QuadTreeNode {
    Bounds bounds;
    BoundarySegment* segments;
    int segment_count;
    struct QuadtreeNode *children[4];
} QuadTreeNode;

#define MAX_DEPTH 8
#define MAX_SEGMENTS_PER_NODE 10
#endif