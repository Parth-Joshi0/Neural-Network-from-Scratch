#ifndef TRACK_INTERNALS_H
#define TRACK_INTERNALS_H

#include "types.h"
#include "track_loader.h"

typedef struct {
    int count;
    Point *points;
} Boundary;

typedef struct {
    Point start;
    Point end;
    Vector2d normal;
    float length;
} BoundarySegment;

struct Track {
    float width;
    Boundary left_boundary;
    Boundary right_boundary;
    int num_boundary_segments;
    BoundarySegment *left_boundary_segments;
    BoundarySegment *right_boundary_segments;
    float *cumulative_length;
    float total_length;
};

#endif