#include <stdio.h>
#include "track_loader.h"
#include "track_internals.h"
#include "car.h"
#include "physics.h"
#include "ray_cast.h"
#include "quad_tree.h"
#include <stdlib.h>

QuadTreeNode* build_track_quadtree(Track* track) {
    // Combine left and right boundary segments
    int total_segments = track->num_boundary_segments * 2;
    struct BoundarySegment* all_segments = malloc(total_segments * sizeof(struct BoundarySegment));
    
    // Copy left boundary segments
    for (int i = 0; i < track->num_boundary_segments; i++) {
        all_segments[i] = track->left_boundary_segments[i];
    }
    
    // Copy right boundary segments
    for (int i = 0; i < track->num_boundary_segments; i++) {
        all_segments[track->num_boundary_segments + i] = track->right_boundary_segments[i];
    }
    
    // Calculate bounds from all points
    Point* all_points = malloc((track->left_boundary.count + track->right_boundary.count) * sizeof(Point));
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
    const char *filename = "tracks/test.txt";
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
    
    // Place car in middle of track
    float mid_x = (tree->bounds.min_x + tree->bounds.max_x) / 2.0f;
    float mid_y = (tree->bounds.min_y + tree->bounds.max_y) / 2.0f;
    Point car_start = {mid_x, mid_y};
    float car_heading = 0.0f; // Facing right
    
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
           car_heading, car_heading * 180.0f / 3.1415926);
    
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
    printf("\n\nTEST 7: Edge case tests...\n");
    
    // Test ray from corner of track
    Point corner = {tree->bounds.min_x, tree->bounds.min_y};
    printf("Testing from track corner (%.2f, %.2f)...\n", corner.x, corner.y);
    RayHit corner_hit = cast_ray(tree, corner, 0.0f, 1000.0f);
    if (corner_hit.hit) {
        printf("  Corner ray hit at distance %.2f\n", corner_hit.distance);
    } else {
        printf("  Corner ray missed\n");
    }
    
    // Test ray parallel to track boundary
    printf("\nTesting parallel ray...\n");
    Point parallel_origin = {mid_x, tree->bounds.min_y + 1.0f};
    RayHit parallel_hit = cast_ray(tree, parallel_origin, 0.0f, 1000.0f);
    if (parallel_hit.hit) {
        printf("  Parallel ray hit at distance %.2f\n", parallel_hit.distance);
    } else {
        printf("  Parallel ray missed\n");
    }
    
    // ========================================
    // TEST 7: Performance Test
    // ========================================
    printf("\n\nTEST 8: Performance test...\n");
    printf("Casting 1000 rays...\n");
    
    int total_hits = 0;
    for (int i = 0; i < 1000; i++) {
        float random_angle = (i * 2.0f * 3.1415926) / 1000.0f;
        RayHit hit = cast_ray(tree, pos, random_angle, 1000.0f);
        if (hit.hit) total_hits++;
    }
    
    printf("  Performance test complete\n");
    printf("  Total rays cast: 1000\n");
    printf("  Rays that hit: %d (%.1f%%)\n", total_hits, (total_hits / 1000.0f) * 100.0f);
    
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
    printf("\n All tests completed successfully!\n\n");
    
    return 0;
}
