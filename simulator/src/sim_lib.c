#include "sim_lib.h"
#include "physics.h"
#include "ray_cast.h"
#include "quad_tree.h"
#include "util.h"
#include <stdlib.h>
#include "track_collision.h"
#include "car.h"
#include "car_internals.h"
#include "physics_constants.h"
#include "track_internals.h"

#define MAX_SIM_STEPS 500

static Track* track = NULL;
static QuadTreeNode* tree = NULL;
static Car* car = NULL;
static Point start_point;
static float start_heading;
static float prev_distance_traveled;
static int prev_furthest_point_index = 0;
static int sim_num = 0;

static void cast_rays();
static void update_furthest_point_index();

int sim_init(const char* track_filename, float car_start_x, float car_start_y, float car_start_heading) {
    track = load_track(track_filename);
    if (track == NULL) {
        return 1;
    }

    tree = build_track_quadtree(track);

    start_point.x = car_start_x;
    start_point.y = car_start_y;
    start_heading = car_start_heading;

    car = create_car(start_point, start_heading);
    cast_rays();
    prev_distance_traveled = 0;
    prev_furthest_point_index = 0;
    return 0;
}

void sim_reset(float* state_out) {
    reset_car(car, start_point, start_heading);
    cast_rays();
    prev_distance_traveled = 0;
    prev_furthest_point_index = 0;
    sim_num = 0;
    for (int i = 0; i < 9; i++) {
        state_out[i] = car->ray_distances[i] / MAX_RAY_DISTANCE;
    }
    state_out[9] = car->speed / MAX_FORWARD_SPEED;
    state_out[10] = car->acceleration / MAX_ACCELERATION;
    state_out[11] = car->steering_angle / MAX_STEERING_ANGLE;
}

void sim_step(float delta_accel, float delta_steering, float* state_out, float* reward_out, int* alive_out, int* success_out) {
    sim_num++;
    update_car_physics(car, delta_accel + car->acceleration, delta_steering + car->steering_angle, 1.0f);
    cast_rays();
    check_car_collision(car, tree);
    update_furthest_point_index();

    for (int i = 0; i < 9; i++) {
        state_out[i] = car->ray_distances[i] / MAX_RAY_DISTANCE;
    }
    state_out[9] = car->speed / MAX_FORWARD_SPEED;
    state_out[10] = car->acceleration / MAX_ACCELERATION;
    state_out[11] = car->steering_angle / MAX_STEERING_ANGLE;

    *reward_out = track->cumulative_length[car->furthest_point_index] - track->cumulative_length[prev_furthest_point_index] - (sim_num * 0.0001f);
    prev_distance_traveled = car->total_distance_traveled;
    if (sim_num >= MAX_SIM_STEPS) {
        car->is_alive = false;
    }
    *alive_out = car->is_alive;
    *success_out = (car->furthest_point_index >= track->left_boundary.count - 2);
}

void sim_get_state(float* state_out) {
    for (int i = 0; i < 9; i++) {
        state_out[i] = car->ray_distances[i] / MAX_RAY_DISTANCE;
    }
    state_out[9] = car->speed / MAX_FORWARD_SPEED;
    state_out[10] = car->acceleration / MAX_ACCELERATION;
    state_out[11] = car->steering_angle / MAX_STEERING_ANGLE;
}

void sim_close(void) {
    free_quadtree(tree);
    free_track(track);
    destroy_car(car);
}

static void cast_rays() {
    for (int j = 0; j < NUM_RAYS; j++) {
        car->ray_distances[j] = cast_ray(tree, car->position,car->heading + RAY_ANGLES[j], MAX_RAY_DISTANCE).distance;
    }
}

static void update_furthest_point_index() {
    prev_furthest_point_index = car->furthest_point_index;
    float padding = 5.0f;
    Bounds query_bounds = {
        car->position.x - padding,
        car->position.y - padding,
        car->position.x + padding,
        car->position.y + padding
    };

    struct BoundarySegment results[MAX_COLLISION_CHECKS];
    int count = 0;
    query_region(tree, &query_bounds, results, &count, MAX_COLLISION_CHECKS);

    float min_dist = 1e30f;
    struct BoundarySegment* nearest = NULL;

    for (int i = 0; i < count; i++) {
        if (results[i].type != BOUNDARY_LEFT) continue;
        float seg_dx = results[i].end.x - results[i].start.x;
        float seg_dy = results[i].end.y - results[i].start.y;
        float to_x = car->position.x - results[i].start.x;
        float to_y = car->position.y - results[i].start.y;
        float t = (to_x * seg_dx + to_y * seg_dy) / (results[i].length * results[i].length);
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        float cx = results[i].start.x + t * seg_dx;
        float cy = results[i].start.y + t * seg_dy;
        float dx = car->position.x - cx;
        float dy = car->position.y - cy;
        float d = dx * dx + dy * dy;
        if (d < min_dist) {
            min_dist = d;
            nearest = &results[i];
        }
    }

    if (nearest == NULL) return;

    for (int i = 0; i < track->num_boundary_segments; i++) {
        if (track->left_boundary_segments[i].start.x == nearest->start.x &&
            track->left_boundary_segments[i].start.y == nearest->start.y) {
            if (i > car->furthest_point_index) {
                car->furthest_point_index = i;
            }
            break;
        }
    }
}
