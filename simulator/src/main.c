#include "window.h"
#include "shader.h"
#include "track_renderer.h"
#include <stdio.h>
#include "car_renderer.h"
#include "ray_renderer.h"
#include "physics.h"
#include "ray_cast.h"
#include "quad_tree.h"
#include "util.h"
#include <stdlib.h>
#include "track_collision.h"

#define TEST_CAR_COUNT 1

void spawn_test_cars(Car* cars[TEST_CAR_COUNT]);
void free_test_cars(Car* cars[], int count);
void move_cars(Car* cars[], int count);
void cast_rays(Car* car[]);
QuadTreeNode* build_track_quadtree(Track* track);

static QuadTreeNode* tree;

int main(void) {
    
    if (window_init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE) == -1) {
        return 1;
    }

    const char *filename = "tracks/track_001.txt";
    Track *track = load_track(filename);
    if (track == NULL) {
        printf("Failed to load track: %s\n", filename);
        return 1;
    }
    
    if(track_renderer_init(&track->left_boundary, &track->right_boundary)) {
        free_track(track);
        fprintf(stderr, "Failed to render Track \n");
        return 1;
    }

    tree = build_track_quadtree(track);

    Car* cars[TEST_CAR_COUNT];
    spawn_test_cars(cars);
    if (car_renderer_init(TEST_CAR_COUNT)) {
        free_track(track);
        free_test_cars(cars, TEST_CAR_COUNT);
        free_quadtree(tree);
        track_renderer_cleanup();
        fprintf(stderr, "Failed to render cars \n");
    }

    if (ray_renderer_init()) {
        free_track(track);
        free_test_cars(cars, TEST_CAR_COUNT);
        free_quadtree(tree);
        car_renderer_cleanup();
        track_renderer_cleanup();
        fprintf(stderr, "Failed to render cars \n");
    }


    while (!window_should_close() && cars[0]->is_alive == true) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        cast_rays(cars);

        car_renderer_update(TEST_CAR_COUNT, cars);
        car_renderer_draw();

        track_renderer_draw();

        ray_renderer_update(cars[0]);
        ray_renderer_draw();
        
        move_cars(cars, TEST_CAR_COUNT);

        check_car_collision(cars[0], tree);

        window_swap_and_poll();
    }

    track_renderer_cleanup();
    car_renderer_cleanup();
    ray_renderer_cleanup();
    free_quadtree(tree);
    free_track(track);
    free_test_cars(cars, TEST_CAR_COUNT);
    window_cleanup();

    return 0;
}

void spawn_test_cars(Car* cars[TEST_CAR_COUNT]) {
    Point starts[TEST_CAR_COUNT] = {
        {13.0f, 12.3f},
        //{20.0f, 18.0f},
        //{35.5f, 40.2f},
        //{50.0f, 10.0f},
        //{75.0f, 60.0f}
    };

    float headings[TEST_CAR_COUNT] = {
        0.0f,
        //0.5f,
        //1.0f,
        //1.57f,
        //3.14f
    };

    for (int i = 0; i < TEST_CAR_COUNT; i++) {
        cars[i] = create_car(starts[i], headings[i]);
    }
}

void free_test_cars(Car* cars[], int count) {
    for (int i = 0; i < count; i++) {
        if (cars[i] != NULL) {
            destroy_car(cars[i]);
            cars[i] = NULL;
        }
    }
}

void move_cars(Car* cars[], int count) {
    for (int i = 0; i < count; i++) {
        if (cars[i] != NULL) {
            update_car_physics(cars[i], 1.0f, 0.00f, 0.01f);
        }
    }
}

void cast_rays(Car* car[]) {
    for (int j = 0; j < NUM_RAYS; j++) {
        car[0]->ray_distances[j] = cast_ray(tree, car[0]->position,car[0]->heading + RAY_ANGLES[j], MAX_RAY_DISTANCE).distance;
    }
}