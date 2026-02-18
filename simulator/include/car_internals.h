#ifndef CAR_INTERNAL_H
#define CAR_INTERNAL_H

#include "car.h"
#include <stdbool.h>

struct Car {
    Point position;
    Vector2d velocity;
    float speed; // scalar magnitude of velocity
    float acceleration;
    float steering_angle; // Angle in radians representing the car's current steering angle
    float heading; // Angle in radians representing the car's current direction
    float angular_velocity;

    float ray_distances[NUM_RAYS];

    int furthest_point_index; // Reward Function
    float total_distance_traveled;
    float time_alive;

    int no_movement_frames; // counter for stationary frames
    bool is_alive;
};

#endif