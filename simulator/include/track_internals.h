#ifndef TRACK_INTERNALS_H
#define TRACK_INTERNALS_H

#include "types.h"
#include "track_loader.h"

typedef struct {
    int count;
    Point *points;
} Boundary;

typedef enum {
    BOUNDARY_LEFT, 
    BOUNDARY_RIGHT
} BoundaryType;

struct BoundarySegment {
    Point start;
    Point end;
    Vector2d normal; // Pointing Inwards toward track center
    float length;
    BoundaryType type;
} BoundarySegment;

struct Track {
    float width;
    Boundary left_boundary;
    Boundary right_boundary;
    int num_boundary_segments;
    struct BoundarySegment *left_boundary_segments;
    struct BoundarySegment *right_boundary_segments;
    float *cumulative_length;
    float total_length;
};

#endif