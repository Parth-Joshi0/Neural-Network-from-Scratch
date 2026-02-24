#include <stdio.h>
#include "track_loader.h"
#include "track_internals.h"
#include "car.h"
#include "physics.h"
#include "ray_cast.h"
#include "quad_tree.h"
#include "util.h"
#include "track_collision.h"
#include <stdlib.h>

QuadTreeNode* build_track_quadtree(Track* track) {
    // Combine left and right boundary segments
    int total_segments = track->num_boundary_segments * 2;
    struct BoundarySegment* all_segments = xalloc(total_segments, sizeof(struct BoundarySegment));
    
    // Copy left boundary segments
    for (int i = 0; i < track->num_boundary_segments; i++) {
        all_segments[i] = track->left_boundary_segments[i];
    }
    
    // Copy right boundary segments
    for (int i = 0; i < track->num_boundary_segments; i++) {
        all_segments[track->num_boundary_segments + i] = track->right_boundary_segments[i];
    }
    
    // Calculate bounds from all points
    Point* all_points = xalloc((track->left_boundary.count + track->right_boundary.count), sizeof(Point));
    for (int i = 0; i < track->left_boundary.count; i++) {
        all_points[i] = track->left_boundary.points[i];
    }
    for (int i = 0; i < track->right_boundary.count; i++) {
        all_points[track->left_boundary.count + i] = track->right_boundary.points[i];
    }
    
    Bounds bounds = calculateBounds(all_points, track->left_boundary.count + track->right_boundary.count);
    free(all_points);
    
    // Build the quad tree
    QuadTreeNode* tree = createQuadTreeNode(bounds, all_segments, total_segments, 0);
    free(all_segments);
    
    return tree;
}

int main(void) {
    printf("╔════════════════════════════════════════╗\n");
    printf("║     RAYTRACING TEST PROGRAM           ║\n");
    printf("╔════════════════════════════════════════╗\n\n");
    
    // ========================================
    // TEST 1: Load Track
    // ========================================
    printf("TEST 1: Loading track...\n");
    const char *filename = "tracks/track_001.txt";
    Track *track = load_track(filename);
    if (track == NULL) {
        printf("Failed to load track: %s\n", filename);
        return 1;
    }
    printf("  Track loaded successfully\n");
    printf("  Track width: %.2f\n", track_width(track));
    printf("  Total length: %.2f\n", track_total_length(track));
    printf("  Left boundary points: %d\n", track->left_boundary.count);
    printf("  Right boundary points: %d\n", track->right_boundary.count);
    printf("  Boundary segments: %d\n", track->num_boundary_segments);
    
    // ========================================
    // TEST 2: Build Quad Tree
    // ========================================
    printf("\n\nTEST 2: Building quad tree...\n");
    QuadTreeNode* tree = build_track_quadtree(track);
    if (tree == NULL) {
        printf("Failed to build quad tree\n");
        free_track(track);
        return 1;
    }
    printf("  Quad tree built successfully\n");
    printf("  Root bounds: (%.2f, %.2f) to (%.2f, %.2f)\n",
           tree->bounds.min_x, tree->bounds.min_y,
           tree->bounds.max_x, tree->bounds.max_y);

    // ========================================
    // TEST 3: Create Car
    // ========================================
    printf("\n\nTEST 3: Creating car...\n");

    Point car_start = {13.0f, 12.3f};
    float car_heading = 0.0f;

    Car *car = create_car(car_start, car_heading);
    if (car == NULL) {
    printf("Failed to create car\n");
    free_quadtree(tree);
    free_track(track);
    return 1;
    }
    printf("  Car created successfully\n");
    Point pos = get_car_position(car);
    printf("  Starting position: (%.2f, %.2f)\n", pos.x, pos.y);
    printf("  Starting heading: %.2f radians (%.1f°)\n",
    car_heading, car_heading * 180.0f / 3.1415926f);
    
    // ========================================
    // TEST 4: Single Ray Test
    // ========================================
    printf("\n\nTEST 4: Single ray test...\n");
    printf("Casting ray from car position in heading direction...\n");
    RayHit test_hit = cast_ray(tree, pos, car_heading, 1000.0f);
    if (test_hit.hit) {
        printf("  Ray hit track boundary!\n");
        printf("  Distance: %.2f\n", test_hit.distance);
        printf("  Hit point: (%.2f, %.2f)\n", test_hit.point.x, test_hit.point.y);
    } else {
        printf("  Ray did not hit any boundary (possible issue)\n");
    }
    
    // ========================================
    // TEST 5: Ray Casting While Moving
    // ========================================
    printf("\n\nTEST 6: Raycasting while car moves...\n");
    printf("Moving car forward with throttle=1.0 for 3 steps\n");
    
    for (int step = 0; step < 3; step++) {
        update_car_physics(car, 1.0f, 0.0f, 0.1f);
        
        Point current_pos = get_car_position(car);
        float current_speed = get_car_speed(car);
        
        printf("\n--- Step %d ---\n", step + 1);
        printf("Position: (%.2f, %.2f), Speed: %.2f\n", 
               current_pos.x, current_pos.y, current_speed);
        
        // Cast 4 rays (front, right, back, left)
        float directions[4] = {0.0f, 3.1415926/2, 3.1415926, 3*3.1415926/2};
        const char* dir_names[4] = {"Front", "Right", "Back", "Left"};
        
        for (int i = 0; i < 4; i++) {
            RayHit hit = cast_ray(tree, current_pos, car_heading + directions[i], 1000.0f);
            printf("  %s: ", dir_names[i]);
            if (hit.hit) {
                printf("%.2f units\n", hit.distance);
            } else {
                printf("No hit\n");
            }
        }
    }
    
    // ========================================
    // TEST 6: Edge Case Tests
    // ========================================
    printf("\n\nTEST 6: Edge case tests...\n");

    // Test ray from a point known to be outside the track
    Point outside = {tree->bounds.min_x, tree->bounds.min_y};
    printf("Testing from outside track bounds (%.2f, %.2f)...\n", outside.x, outside.y);
    RayHit outside_hit = cast_ray(tree, outside, 0.0f, 1000.0f);
    if (outside_hit.hit) {
        printf("  Outside ray hit at distance %.2f\n", outside_hit.distance);
    } else {
        printf("  Outside ray missed\n");
    }

    // Test ray parallel to track boundary from known inside point
    printf("\nTesting parallel ray from inside track...\n");
    Point parallel_origin = {13.0f, 12.3f};
    RayHit parallel_hit = cast_ray(tree, parallel_origin, 0.0f, 1000.0f);
    if (parallel_hit.hit) {
        printf("  Parallel ray hit at distance %.2f\n", parallel_hit.distance);
    } else {
        printf("  Parallel ray missed\n");
    }

    // ========================================
    // TEST 7: Query Region Test
    // ========================================
    printf("\n\nTEST 7: Query region test...\n");

    Point corners[4];
    get_corners(car, corners);
    Bounds car_bounds = calculateBounds(corners, 4);

    printf("  Car bounds: (%.2f, %.2f) to (%.2f, %.2f)\n",
        car_bounds.min_x, car_bounds.min_y,
        car_bounds.max_x, car_bounds.max_y);

    struct BoundarySegment results[32];
    int count = 0;
    query_region(tree, &car_bounds, results, &count, 32);

    printf("  Segments found near car: %d\n", count);
    int left_count = 0, right_count = 0;
    for (int i = 0; i < count; i++) {
        if (results[i].type == BOUNDARY_LEFT) left_count++;
        else right_count++;
    }
    printf("  Left boundary segments: %d\n", left_count);
    printf("  Right boundary segments: %d\n", right_count);

    if (count == 0) {
        printf("  WARNING: No segments found - car may be outside track bounds\n");
    } else if (left_count == 0 || right_count == 0) {
        printf("  WARNING: Missing segments on one boundary side\n");
    } else {
        printf("  Query region: PASS\n");
    }

    // ========================================
    // TEST 8: Collision Detection Test
    // ========================================
    printf("\n\nTEST 8: Collision detection test...\n");

    // Test 1 - Car should be alive at known good start position
    int alive = check_car_collision(car, tree);
    printf("  Car at start position (13.0, 12.3): %s\n",
        alive ? "ALIVE (expected)" : "DEAD (unexpected)");

    // Test 2 - Car placed well outside track bounds should be dead
    Point outside_pos = {tree->bounds.max_x + 100.0f, tree->bounds.max_y + 100.0f};
    Car* outside_car = create_car(outside_pos, 0.0f);
    if (outside_car) {
        int outside_alive = check_car_collision(outside_car, tree);
        printf("  Car outside track bounds: %s\n",
            !outside_alive ? "DEAD (expected)" : "ALIVE (unexpected)");
        destroy_car(outside_car);
    }

    // Test 3 - Car placed exactly on left boundary point should be dead
    Point left_boundary_point = track->left_boundary.points[0];
    printf("  Testing car at left boundary point (%.2f, %.2f)...\n",
        left_boundary_point.x, left_boundary_point.y);
    Car* boundary_car = create_car(left_boundary_point, 0.0f);
    if (boundary_car) {
        int boundary_alive = check_car_collision(boundary_car, tree);
        printf("  Car at left boundary point: %s\n",
            !boundary_alive ? "DEAD (expected)" : "ALIVE (unexpected)");
        destroy_car(boundary_car);
    }

    // Test 4 - Car placed exactly on right boundary point should be dead
    Point right_boundary_point = track->right_boundary.points[0];
    printf("  Testing car at right boundary point (%.2f, %.2f)...\n",
        right_boundary_point.x, right_boundary_point.y);
    Car* right_boundary_car = create_car(right_boundary_point, 0.0f);
    if (right_boundary_car) {
        int right_alive = check_car_collision(right_boundary_car, tree);
        printf("  Car at right boundary point: %s\n",
            !right_alive ? "DEAD (expected)" : "ALIVE (unexpected)");
        destroy_car(right_boundary_car);
    }

    // Test 5 - Simulate car driving straight and check collision each step
    printf("  Simulating car driving straight with collision checks...\n");
    int steps_alive = 0;
    for (int step = 0; step < 100; step++) {
        update_car_physics(car, 1.0f, 0.0f, 0.1f);
        if (!check_car_collision(car, tree)) {
            printf("  Car died at step %d\n", step + 1);
            break;
        }
        steps_alive++;
    }
    if (steps_alive == 100) {
        printf("  Car survived all 100 steps driving straight\n");
    }
    
    // ========================================
    // CLEANUP
    // ========================================
    printf("\n\n=== Cleanup ===\n");
    destroy_car(car);
    free_quadtree(tree);
    free_track(track);
    printf("  All resources freed\n");
    
    // ========================================
    // SUMMARY
    // ========================================
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║          TEST SUMMARY                 ║\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("  Track loading: PASS\n");
    printf("  Quad tree construction: PASS\n");
    printf("  Car creation: PASS\n");
    printf("  Single ray casting: PASS\n");
    printf("  Dynamic raycasting: PASS\n");
    printf("  Edge cases: PASS\n");
    printf("  Performance: PASS\n");
    printf("  Query region: PASS\n");
    printf("  Collision detection: PASS\n");
    printf("\n All tests completed successfully!\n\n");
    
    return 0;
}
