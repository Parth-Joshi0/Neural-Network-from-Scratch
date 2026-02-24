#ifndef QUADTREE_H
#define QUADTREE_H

#include "types.h"
#include "track_internals.h"

typedef struct {
    float min_x;
    float min_y;
    float max_x;
    float max_y;
} Bounds;

typedef struct QuadTreeNode {
    Bounds bounds;
    struct BoundarySegment* segments;
    int segment_count;
    struct QuadTreeNode *children[4];
} QuadTreeNode;

Bounds calculateBounds(Point* points, int count);
QuadTreeNode* createQuadTreeNode(Bounds bounds, struct BoundarySegment* segment, int segment_count, int depth);
void free_quadtree(QuadTreeNode* node);
int segmentIntersectsBound(Bounds* bound, struct BoundarySegment* segment);
int pointIntersectsBound(Point p, Bounds* bound);
void query_region(QuadTreeNode* node, Bounds* region, struct BoundarySegment* results, int* count, int max_results);

#define MAX_DEPTH 10
#define MAX_SEGMENTS_PER_NODE 30
#endif