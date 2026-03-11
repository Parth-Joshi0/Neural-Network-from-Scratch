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

static Track* track = NULL;
static QuadTreeNode* tree = NULL;
static Car* car = NULL;
static Point start_point;
static float start_heading;
static float prev_distance_traveled;

static void cast_rays();

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
    return 0;
}


void sim_reset(float* state_out) {
    reset_car(car, start_point, start_heading);
    cast_rays();
    prev_distance_traveled = 0;
    for (int i = 0; i < 9; i++) {
        state_out[i] = car->ray_distances[i] / MAX_RAY_DISTANCE;
    }
    state_out[9] = car->speed / MAX_FORWARD_SPEED;
    state_out[10] = car->acceleration / MAX_ACCELERATION;
    state_out[11] = car->steering_angle / MAX_STEERING_ANGLE;
}

void sim_step(float delta_accel, float delta_steering, float* state_out, float* reward_out, int* alive_out, int* success_out) {
    update_car_physics(car, delta_accel + car->acceleration, delta_steering + car->steering_angle, 0.1f);
    cast_rays();
    check_car_collision(car, tree);

    for (int i = 0; i < 9; i++) {
        state_out[i] = car->ray_distances[i] / MAX_RAY_DISTANCE;
    }
    state_out[9] = car->speed / MAX_FORWARD_SPEED;
    state_out[10] = car->acceleration / MAX_ACCELERATION;
    state_out[11] = car->steering_angle / MAX_STEERING_ANGLE;

    *reward_out = car->total_distance_traveled - prev_distance_traveled;
    prev_distance_traveled = car->total_distance_traveled;
    *alive_out = car->is_alive;
    *success_out = (car->furthest_point_index >= track->num_boundary_segments - 1);
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
