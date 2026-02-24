#include "quad_tree.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"

Bounds calculateBounds(Point* points, int count){
    Bounds b;
    b.min_x = b.max_x = points[0].x;
    b.min_y = b.max_y = points[0].y;

    for (int i = 1; i < count; i++) {
        if (points[i].x > b.max_x) b.max_x = points[i].x;
        if (points[i].x < b.min_x) b.min_x = points[i].x;
        if (points[i].y > b.max_y) b.max_y = points[i].y;
        if (points[i].y < b.min_y) b.min_y = points[i].y;
    }

    return b;
}

int segmentIntersectsBound(Bounds* bound, struct BoundarySegment* segment){
    // Check if either enpoint is in the bound
    if (pointIntersectsBound(segment->start, bound) || pointIntersectsBound(segment->end, bound))
        return 1;

    // Bounding box overlap test
    float seg_min_x = fminf(segment->start.x, segment->end.x);
    float seg_min_y = fminf(segment->start.y, segment->end.y);
    float seg_max_x = fmaxf(segment->start.x, segment->end.x);
    float seg_max_y = fmaxf(segment->start.y, segment->end.y);

    if (seg_max_x < bound->min_x || seg_min_x > bound->max_x)
        return 0;
    if (seg_max_y < bound->min_y || seg_min_y > bound->max_y)
        return 0;

    return 1;
}

int pointIntersectsBound(Point p, Bounds* bound) {
    // Check if point lies within the segment
    if ((p.x >= bound->min_x && p.x <= bound->max_x) && (p.y >= bound->min_y && p.y <= bound->max_y))
        return 1;
    return 0;
}

QuadTreeNode* createQuadTreeNode(Bounds bounds, struct BoundarySegment* segment, int segment_count, int depth){
    QuadTreeNode* node = xalloc(1, sizeof(QuadTreeNode));
    node->bounds = bounds;
    node->segment_count = 0;
    node->segments = NULL;
    node->children[0] = node->children[1] = node->children[2] = node->children[3] = NULL;

    // Leaf Node
    if (segment_count <= MAX_SEGMENTS_PER_NODE || depth >= MAX_DEPTH) {
        node->segments = xalloc(segment_count, sizeof(BoundarySegment));
        memcpy(node->segments, segment, segment_count * sizeof(BoundarySegment));
        node->segment_count = segment_count;
        return node;
    }

    // Sub Divide Node
    float mid_x = (bounds.min_x + bounds.max_x) / 2.0f;
    float mid_y = (bounds.min_y + bounds.max_y) / 2.0f;

    Bounds child_Bounds[4] = {
        {bounds.min_x, bounds.min_y, mid_x, mid_y}, // bottom Left
        {mid_x, bounds.min_y, bounds.max_x, mid_y}, // bottom right
        {bounds.min_x, mid_y, mid_x, bounds.max_y}, // top left
        {mid_x, mid_y, bounds.max_x, bounds.max_y} // top right
    };

    for (int i= 0; i<4; i++) {
        struct BoundarySegment* child_segments = xalloc(segment_count, sizeof(BoundarySegment));
        int child_count = 0;

        for (int j = 0; j < segment_count; j++) {
            if (segmentIntersectsBound(&child_Bounds[i], &segment[j])) {
                child_segments[child_count++] = segment[j];
            }
        }

        if (child_count > 0) {
            node -> children[i] = createQuadTreeNode(child_Bounds[i], child_segments, child_count, depth + 1);
        }
        
        free(child_segments);
    }
    
    return node;
}

void free_quadtree(QuadTreeNode* node) {
    if (!node) {
        return;
    }

    if (node->segments) {
        free(node->segments);
    }

    for (int i = 0; i < 4; i++) {
        free_quadtree(node->children[i]);
    }

    free(node);
}

int boundIntersectsBounds(Bounds* region1, Bounds* region2){
    if (region1 == NULL || region2 == NULL){
        return 0;
    }

    if (region1->max_x < region2->min_x) return 0; // region1 left of region2
    if (region2->max_x < region1->min_x) return 0; // region2 left of region1
    if (region1->max_y < region2->min_y) return 0; // region1 below region2
    if (region2->max_y < region1->min_y) return 0; // region2 below region1

    return 1;
}

void query_region(QuadTreeNode* node, Bounds* region, struct BoundarySegment* results, int* count, int max_results) {
    // Recursively queries the quadtree for BoundarySegments intersecting the given
    // region. Uses node-bound pruning, tests segments in leaf nodes, avoids
    // duplicates, and appends matches to results up to max_results.

    if (node == NULL || !boundIntersectsBounds(&node->bounds, region)) {
        return;
    }

    if (node->children[0] == NULL) {
        // Leaf Node
        for (int i = 0; i < node->segment_count; i++) {
            struct BoundarySegment segment = node->segments[i];
            if (segmentIntersectsBound(region, &segment)) {
                int is_duplicate = 0;

                for (int j = 0; j < *count; j++) {
                    if (results[j].start.x == segment.start.x &&
                        results[j].start.y == segment.start.y &&
                        results[j].end.x == segment.end.x &&
                        results[j].end.y == segment.end.y) {

                        is_duplicate = 1;
                        break;
                    }
                }

                if (!is_duplicate && *count < max_results) {
                    results[*count] = segment;
                    (*count)++;
                }
             }
        }
    }
    else {
        // Branch Node
        for (int i = 0; i < 4; i++) {
            if (node->children[i]) {
                if (*count >= max_results) return;
                query_region(node->children[i], region, results, count, max_results);
            }
        }
    }

    return;
}