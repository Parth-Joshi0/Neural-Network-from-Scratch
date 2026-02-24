#include <track_collision.h>
#include "types.h"
#include "car.h"
#include "car_internals.h"
#include "quad_tree.h"
#include <math.h>
#include <stdlib.h>

void get_corners(Car* car, Point* corners);
int check_car_collision(Car* car, QuadTreeNode* node);
static inline float distance_to_point_segment_sq(float seg_dx, float seg_dy, Point corner, struct BoundarySegment* seg);
static inline int corner_is_on_wrong_side(Point corner, const struct BoundarySegment* seg);

void get_corners(Car* car, Point* corners) {
    // Returns the corners in the order of Front Right (FR), Back Right (BR), Back Left (BL), Front Left (FL)
    const float c = cosf(car->heading);
    const float s = sinf(car->heading);

    const float dx[4] = { +CAR_HALF_WIDTH, +CAR_HALF_WIDTH, -CAR_HALF_WIDTH, -CAR_HALF_WIDTH };
    const float dy[4] = { +CAR_HALF_LENGTH, -CAR_HALF_LENGTH, -CAR_HALF_LENGTH, +CAR_HALF_LENGTH };

    for (int i = 0; i < 4; i++) {
        float rx = dx[i] * c - dy[i] * s;
        float ry = dx[i] * s + dy[i] * c;

        corners[i].x = car->position.x + rx;
        corners[i].y = car->position.y + ry;
    }
}

int check_car_collision(Car* car, QuadTreeNode* node) {
    // returns 0 for dead, 1 for alive

    Point corners[4];
    get_corners(car, &corners[0]);
    Bounds car_bounds = calculateBounds(&corners[0], 4);
    float padding = 2.5f;
    Bounds query_bounds = { // Inflating bound size to ensure nearest segment on each side of the bounding box, regardless of side the car is on
        car_bounds.min_x - padding,
        car_bounds.min_y - padding,
        car_bounds.max_x + padding,
        car_bounds.max_y + padding
    };


    // Finds Nearby Segments to Car bounds
    struct BoundarySegment results[MAX_COLLISION_CHECKS];
    int count = 0;
    query_region(node, &query_bounds, &results[0], &count, MAX_COLLISION_CHECKS);

    // Finds nearest segment to each car corner
    struct BoundarySegment *lFR = NULL, *lBR = NULL, *lBL = NULL, *lFL = NULL;
    struct BoundarySegment *rFR = NULL, *rBR = NULL, *rBL = NULL, *rFL = NULL;

    float lmin_FR = 1e30f, lmin_BR = 1e30f, lmin_BL = 1e30f, lmin_FL = 1e30f;
    float rmin_FR = 1e30f, rmin_BR = 1e30f, rmin_BL = 1e30f, rmin_FL = 1e30f;

    for (int i = 0; i < count; i++) {
        struct BoundarySegment* seg = &results[i];

        // Vector along the segment from start to end
        float seg_dx = seg->end.x - seg->start.x;
        float seg_dy = seg->end.y - seg->start.y;

        if (seg->length <= 1e-6f) continue;

        if (seg->type == BOUNDARY_LEFT) {
            float d;

            // Front-Right
            d = distance_to_point_segment_sq(seg_dx, seg_dy, corners[0], seg);
            if (d < lmin_FR) { lmin_FR = d; lFR = seg; }

            // Back-Right
            d = distance_to_point_segment_sq(seg_dx, seg_dy, corners[1], seg);
            if (d < lmin_BR) { lmin_BR = d; lBR = seg; }

            // Back-Left
            d = distance_to_point_segment_sq(seg_dx, seg_dy, corners[2], seg);
            if (d < lmin_BL) { lmin_BL = d; lBL = seg; }

            // Front-Left
            d = distance_to_point_segment_sq(seg_dx, seg_dy, corners[3], seg);
            if (d < lmin_FL) { lmin_FL = d; lFL = seg; }
        }
        else {
            float d;

            // Front-Right
            d = distance_to_point_segment_sq(seg_dx, seg_dy, corners[0], seg);
            if (d < rmin_FR) { rmin_FR = d; rFR = seg; }

            // Back-Right
            d = distance_to_point_segment_sq(seg_dx, seg_dy, corners[1], seg);
            if (d < rmin_BR) { rmin_BR = d; rBR = seg; }

            // Back-Left
            d = distance_to_point_segment_sq(seg_dx, seg_dy, corners[2], seg);
            if (d < rmin_BL) { rmin_BL = d; rBL = seg; }

            // Front-Left
            d = distance_to_point_segment_sq(seg_dx, seg_dy, corners[3], seg);
            if (d < rmin_FL) { rmin_FL = d; rFL = seg; }
        }
    }

    if (corner_is_on_wrong_side(corners[0], lFR) || corner_is_on_wrong_side(corners[0], rFR) ||
    corner_is_on_wrong_side(corners[1], lBR) || corner_is_on_wrong_side(corners[1], rBR) ||
    corner_is_on_wrong_side(corners[2], lBL) || corner_is_on_wrong_side(corners[2], rBL) ||
    corner_is_on_wrong_side(corners[3], lFL) || corner_is_on_wrong_side(corners[3], rFL)) {

        car->is_alive = false;
        return 0;
    }

    return 1;
}

static inline float distance_to_point_segment_sq(float seg_dx, float seg_dy, Point corner, struct BoundarySegment* seg) {
    // Returns the perpendicular distance to point squared

    // Vector from segment start to the corner
    float to_corner_x = corner.x - seg->start.x;
    float to_corner_y = corner.y - seg->start.y;

    // Project corner vector onto segment vector (dot product / length squared)
    // This gives t, how far along the segment the closest point is
    float t = (to_corner_x * seg_dx + to_corner_y * seg_dy) / (seg->length * seg->length);

    // Clamp t between 0 and 1 so we stay within the segment endpoints
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    // Closest point on the segment to the corner
    float closest_x = seg->start.x + t * seg_dx;
    float closest_y = seg->start.y + t * seg_dy;

    // Distance from corner to closest point
    float diff_x = corner.x - closest_x;
    float diff_y = corner.y - closest_y;

    return diff_x * diff_x + diff_y * diff_y;
}

static inline int corner_is_on_wrong_side(Point corner, const struct BoundarySegment* seg) {
    if (seg == NULL) {
        return 1;
    }

    // Vector from segment start to corner
    float vx = corner.x - seg->start.x;
    float vy = corner.y - seg->start.y;

    // Dot with inward normal
    float dot = vx * seg->normal.x + vy * seg->normal.y;

    // Negative => wrong side
    return (dot < 0.0f);
}