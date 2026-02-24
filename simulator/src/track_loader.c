#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "types.h"
#include "track_loader.h"
#include "track_internals.h"
#include "util.h"

static void track_create_segments(Track *track);
static void track_calculate_lengths(Track *track);
void free_track(Track *track);
void *xalloc(size_t num, size_t size);

Track* load_track(const char *filename) {
    Track *track = xalloc(1, sizeof(Track));

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }

    char line[64];
    float track_width;

    fgets(line, sizeof(line), file); // Read track width
    sscanf(line, "WIDTH %f", &track_width);

    if (track_width <= 0) {
        fprintf(stderr, "Error: Invalid track width in file %s\n", filename);
        goto CLEANUP;
    }

    track->width = track_width;

    int segments;
    fgets(line, sizeof(line), file); // Read number of segments
    sscanf(line, "SEGMENTS %d", &segments);

    fgets(line, sizeof(line), file); // Read Empty line
    if (line[0] != '\n') {
        fprintf(stderr, "Expected EMPTY LINE after number of segments\n");
        goto CLEANUP;
    }

    for (int i = 0; i < segments; i++) {
        fgets(line, sizeof(line), file); // SEGMENT
        if (strncmp(line, "SEGMENT", 7) != 0) {
            fprintf(stderr, "Expected SEGMENT\n");
            goto CLEANUP;
        }

        fgets(line, sizeof(line), file); // CONTROL_POINTS
        int count;
        if (sscanf(line, "CONTROL_POINTS %d", &count) != 1) {
            fprintf(stderr, "Reading Control Points Failed\n");
            goto CLEANUP;
        }

        if (count != 4) {
            fprintf(stderr, "Expected 4 control points for segment %d, got %d\n", i+1, count);
            goto CLEANUP;
        }

        for (int j = 0; j < count; j++) {
            fgets(line, sizeof(line), file);
            float x, y;
            if (sscanf(line, "%f %f", &x, &y) != 2) {
                fprintf(stderr, "Expected control point coordinates\n");
                goto CLEANUP;
            }
        }

        fgets(line, sizeof(line), file); // Read Empty line
        if (line[0] != '\n') {
            fprintf(stderr, "Expected EMPTY LINE after Control Points\n");
            goto CLEANUP;
        }
    }

    int left_boundary_points, right_boundary_points;

    fgets(line, sizeof(line), file); // LEFT_BOUNDARY
    sscanf(line, "LEFT_BOUNDARY %d", &left_boundary_points);
    if (left_boundary_points <= 0) {
        fprintf(stderr, "Error: Invalid left boundary points in file %s\n", filename);
        goto CLEANUP;
    }

    track->left_boundary.count = left_boundary_points;
    track->left_boundary.points = xalloc(left_boundary_points, sizeof(Point));

    for (int i = 0; i < left_boundary_points; i++) { // Read left boundary points
        fgets(line, sizeof(line), file);
        if (sscanf(line, "%f %f", &track->left_boundary.points[i].x, &track->left_boundary.points[i].y) != 2) {
            fprintf(stderr, "Expected left boundary point coordinates\n");
            goto CLEANUP;
        }
    }

    fgets(line, sizeof(line), file); // Read Empty line
    if (line[0] != '\n') {
        fprintf(stderr, "Expected EMPTY LINE after Left Boundary Points\n");
        goto CLEANUP;
    }

    fgets(line, sizeof(line), file); // RIGHT_BOUNDARY
    sscanf(line, "RIGHT_BOUNDARY %d", &right_boundary_points);
    if (right_boundary_points <= 0) {
        fprintf(stderr, "Error: Invalid RIGHT boundary points in file %s\n", filename);
        goto CLEANUP;
    }

    track->right_boundary.count = right_boundary_points;
    track->right_boundary.points = xalloc(right_boundary_points, sizeof(Point));

    if (left_boundary_points != right_boundary_points) {
        fprintf(stderr, "Error: Left and right boundary points count mismatch in file %s\n", filename);
        goto CLEANUP;
    }

    for (int i = 0; i < right_boundary_points; i++) { // Read right boundary points
        fgets(line, sizeof(line), file);
        if (sscanf(line, "%f %f", &track->right_boundary.points[i].x, &track->right_boundary.points[i].y) != 2) {
            fprintf(stderr, "Expected left boundary point coordinates\n");
            goto CLEANUP;
        }
    }

    fclose(file);

    track_create_segments(track);
    track_calculate_lengths(track);

    return track;

    CLEANUP:
    free_track(track);
    fclose(file);
    return NULL;

}

static void track_create_segments(Track *track) {
    int left_segs = track->left_boundary.count - 1;
    int right_segs = track->right_boundary.count - 1;
    track->num_boundary_segments = left_segs + right_segs;

    track->left_boundary_segments = xalloc(left_segs, sizeof(struct BoundarySegment));
    track->right_boundary_segments = xalloc(right_segs, sizeof(struct BoundarySegment));

    for (int i =0; i < left_segs; i++) {
        struct BoundarySegment *left_segment = &track->left_boundary_segments[i];
        left_segment->start = track->left_boundary.points[i];
        left_segment->end = track->left_boundary.points[i + 1];

        left_segment->type = BOUNDARY_LEFT;

        float ldx = left_segment->end.x - left_segment->start.x; // Calculate segment length
        float ldy = left_segment->end.y - left_segment->start.y;
        left_segment->length = sqrtf(ldx * ldx + ldy * ldy);

        if (left_segment->length > 0) { // Calculate normal vector (pointing inwards to track center)
            left_segment->normal.x = -ldy / left_segment->length;
            left_segment->normal.y = ldx / left_segment->length;

            // Grab a reference point we know is inside the track (corresponding right boundary point)
            Point interior_ref = track->right_boundary.points[i];

            // Vector from segment start to the interior reference point
            float to_interior_x = interior_ref.x - left_segment->start.x;
            float to_interior_y = interior_ref.y - left_segment->start.y;

            // Dot product: if negative, normal points away from interior, so flip it
            float dot = to_interior_x * left_segment->normal.x + to_interior_y * left_segment->normal.y;
            if (dot < 0) {
                left_segment->normal.x = -left_segment->normal.x;
                left_segment->normal.y = -left_segment->normal.y;
            }
        } else {
            left_segment->normal.x = 0;
            left_segment->normal.y = 0;
        }

        struct BoundarySegment *right_segment = &track->right_boundary_segments[i];
        right_segment->start = track->right_boundary.points[i];
        right_segment->end = track->right_boundary.points[i + 1];
        
        right_segment->type = BOUNDARY_RIGHT;

        float rdx = right_segment->end.x - right_segment->start.x; // Calculate segment length
        float rdy = right_segment->end.y - right_segment->start.y;
        right_segment->length = sqrtf(rdx * rdx + rdy * rdy);

        if (right_segment->length > 0) { // Calculate normal vector (pointing inwards to track center)
            right_segment->normal.x = -rdy / right_segment->length;
            right_segment->normal.y = rdx / right_segment->length;

            // Grab a reference point we know is inside the track (corresponding left boundary point)
            Point interior_ref = track->left_boundary.points[i];

            // Vector from segment start to the interior reference point
            float to_interior_x = interior_ref.x - right_segment->start.x;
            float to_interior_y = interior_ref.y - right_segment->start.y;
                
            // Dot product: if negative, normal points away from interior, so flip it
            float dot = to_interior_x * right_segment->normal.x + to_interior_y * right_segment->normal.y;
            if (dot < 0) {
                right_segment->normal.x = -right_segment->normal.x;
                right_segment->normal.y = -right_segment->normal.y;
            }
        } else {
            right_segment->normal.x = 0;
            right_segment->normal.y = 0;
        }
    }
}

static void track_calculate_lengths(Track *track) {
    track->cumulative_length = xalloc(track->left_boundary.count, sizeof(float));
    track->cumulative_length[0] = 0.0f;  // Start is at distance 0
    for (int i = 1; i < track->left_boundary.count; i++) {
        float dx = track->left_boundary.points[i].x - track->left_boundary.points[i-1].x;
        float dy = track->left_boundary.points[i].y - track->left_boundary.points[i-1].y;
        float dist = sqrtf(dx*dx + dy*dy);
        track->cumulative_length[i] = track->cumulative_length[i-1] + dist;
    }
}

void free_track(Track *track) {
    if (track) {
        free(track->left_boundary.points);
        free(track->right_boundary.points);
        free(track->left_boundary_segments);
        free(track->right_boundary_segments);
        free(track->cumulative_length);
        free(track);
    }
}

float track_width(const Track *t) {
    return t->width;
}

float track_total_length(const Track *t) {
    return t->cumulative_length[t->left_boundary.count - 1];
}