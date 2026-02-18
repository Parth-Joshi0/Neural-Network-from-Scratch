#include "ray_cast.h"
#include "quad_tree.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>

RayHit ray_segment_intersection(Point origin, float direction, struct BoundarySegment* segment);
int ray_intersects_bounds(Point origin, float direction, Bounds* bounds, float max_distance);
RayHit cast_ray(QuadTreeNode* node, Point origin, float direction, float max_distance);

RayHit ray_segment_intersection(Point origin, float direction, struct BoundarySegment* segment) {
    RayHit result = {.hit = 0, .distance = FLT_MAX};

    // convert direction from radians to x, y
    float dx = cosf(direction);
    float dy = sinf(direction);

    // get eqaution of line of segment
    float sx = segment->end.x - segment->start.x;
    float sy = segment->end.y - segment->start.y;

    float determinant = dx * sy - dy * sx;

    // if determinant is ~0, line is parrellel -> no intersection
    if (fabsf(determinant) < 1e-10f)
        return result;

    // Distance from intersection point to intersection
    float t = ((segment->start.x - origin.x) * sy - (segment->start.y - origin.y) * sx) / determinant;
    // Where on the segment the intersection is (0 <= s <= 1 means the segment is within the endpoints)
    float s = ((segment->start.x - origin.x) * dy - (segment->start.y - origin.y) * dx) / determinant;

    // Check if intersection is valid
    if (t >= 0 && s >= 0 && s<= 1) {
        result.hit = 1;
        result.distance = t;

        // Calculate for the points
        result.point.x = origin.x + t * dx;
        result.point.y = origin.y + t * dy;
    }

    return result;
}

int ray_intersects_bounds(Point origin, float direction, Bounds* bounds, float max_distance) {
    // Performs AABB test

    float tmin = 0.0f; //Valid distance of the ray
    float tmax = max_distance;

    float dx = cosf(direction);
    float dy = sinf(direction); 

    // X axis
    if (fabsf(dx) > 1e-10f){
        // Where ray crosses left and right bounds
        float tx1 = (bounds->min_x - origin.x) / dx;
        float tx2 = (bounds->max_x - origin.x) / dx;

        // Push tmin forward to x-entry, pull tmax back to x-exit
        tmin = fmaxf(tmin, fminf(tx1, tx2));
        tmax = fminf(tmax, fmaxf(tx1, tx2));
    } else if (origin.x < bounds->min_x || origin.x > bounds->max_x) {
        // Parrallel and outside the slab
        return 0;
    }

    // Y axis
    if (fabsf(dy) > 1e-10f){
        // Where ray crosses top and bottom walls
        float ty1 = (bounds->min_y - origin.y) / dy;
        float ty2 = (bounds->max_y - origin.y) / dy;

        // Same narrowing of the tmin and tmax range
        tmin = fmaxf(tmin, fminf(ty1, ty2));
        tmax = fminf(tmax, fmaxf(ty1, ty2));
    } else if (origin.y < bounds->min_y || origin.y > bounds->max_y) {
        // Parallel and outside the slab
        return 0;
    }

    // if the entry is still before the exit point, we have a hit
    return tmax >= tmin;
}

RayHit cast_ray(QuadTreeNode* node, Point origin, float direction, float max_distance) {
    RayHit result = {.hit = 0, .distance = max_distance};
    if (!node || !ray_intersects_bounds(origin, direction, &node->bounds, max_distance)) {
        return result;
    }

    if (node->children[0] == NULL) {
        // Leaf Node
        for (int i = 0; i < node->segment_count; i++) {
            RayHit hit = ray_segment_intersection(origin, direction, &node->segments[i]);
            if (hit.hit && hit.distance < result.distance) {
                result = hit;
            }
        }
    } else {
        // Branch Node, Recurse into children
        for (int i = 0; i < 4; i++) {
            if (node->children[i]) {
                RayHit hit = cast_ray(node->children[i], origin, direction, result.distance);
                if (hit.hit && hit.distance < result.distance) {
                    result = hit;
                }
            }
        }
    }

    return result;
}