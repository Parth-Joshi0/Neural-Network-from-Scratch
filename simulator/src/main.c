#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include "shader.h"
#include "track_renderer.h"
#include "car_renderer.h"
#include "ray_renderer.h"
#include "physics.h"
#include "ray_cast.h"
#include "quad_tree.h"
#include "util.h"
#include "track_collision.h"
#include "nn.h"
#include "physics_constants.h"

#define DT 0.01f

static const Point  START_POINT   = {.x = 8.0f, .y = 9.3f};
static const float  START_HEADING = 0.0f;
static QuadTreeNode* tree;

static void cast_rays(Car* car);
static void get_state_from_car(Car* car, float* state);

int main(void) {
    if (window_init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE) == -1) {
        return 1;
    }

    const char* filename = "tracks/test2.txt";
    Track* track = load_track(filename);
    if (track == NULL) {
        fprintf(stderr, "Failed to load track: %s\n", filename);
        window_cleanup();
        return 1;
    }

    if (track_renderer_init(&track->left_boundary, &track->right_boundary)) {
        fprintf(stderr, "Failed to init track renderer\n");
        free_track(track);
        window_cleanup();
        return 1;
    }

    tree = build_track_quadtree(track);

    Car* car = create_car(START_POINT, START_HEADING);

    if (car_renderer_init(1)) {
        fprintf(stderr, "Failed to init car renderer\n");
        destroy_car(car);
        free_quadtree(tree);
        track_renderer_cleanup();
        window_cleanup();
        return 1;
    }

    if (ray_renderer_init()) {
        fprintf(stderr, "Failed to init ray renderer\n");
        destroy_car(car);
        free_quadtree(tree);
        car_renderer_cleanup();
        track_renderer_cleanup();
        window_cleanup();
        return 1;
    }

    Network nn;
    if (nn_load(&nn, "../python/weights.bin") != 0) {
        fprintf(stderr, "Failed to load neural network weights\n");
        destroy_car(car);
        free_quadtree(tree);
        free_track(track);
        ray_renderer_cleanup();
        car_renderer_cleanup();
        track_renderer_cleanup();
        window_cleanup();
        return 1;
    }

    while (!window_should_close()) {
        if (!car->is_alive) {
            reset_car(car, START_POINT, START_HEADING);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        cast_rays(car);

        car_renderer_update(1, &car);
        car_renderer_draw();

        track_renderer_draw();

        ray_renderer_update(car);
        ray_renderer_draw();

        float state[12];
        get_state_from_car(car, state);

        float action[2];
        nn_forward(&nn, state, action);
        update_car_physics(car, action[0] + car->acceleration, action[1] + car->heading, DT);

        cast_rays(car);
        check_car_collision(car, tree);

        window_swap_and_poll();
    }

    track_renderer_cleanup();
    car_renderer_cleanup();
    ray_renderer_cleanup();
    free_quadtree(tree);
    free_track(track);
    destroy_car(car);
    window_cleanup();

    return 0;
}

static void cast_rays(Car* car) {
    for (int j = 0; j < NUM_RAYS; j++) {
        car->ray_distances[j] = cast_ray(
            tree, car->position, car->heading + RAY_ANGLES[j], MAX_RAY_DISTANCE
        ).distance;
    }
}

static void get_state_from_car(Car* car, float* state) {
    for (int i = 0; i < NUM_RAYS; i++)
        state[i] = car->ray_distances[i] / MAX_RAY_DISTANCE;
    state[9]  = car->speed / MAX_FORWARD_SPEED;
    state[10] = car->acceleration / MAX_ACCELERATION;
    state[11] = car->steering_angle / MAX_STEERING_ANGLE;
}
