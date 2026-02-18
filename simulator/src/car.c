#include "car.h"
#include "car_internals.h"
#include "types.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

Car *create_car(Point start_position, float start_direction)
{
    Car *car = calloc(1, sizeof(Car));
    if (car == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for Car.\n");
        return NULL; // Allocation failed
    }

    car->position = start_position;
    car->heading = start_direction;
    car->is_alive = true;

    return car;
}

Car *reset_car(Car *car, Point start_position, float start_direction)
{
    if (car == NULL) {
        return NULL; // Invalid car pointer
    }

    car->position = start_position;
    car->heading = start_direction;
    car->steering_angle = 0.0f;
    car->velocity.x = 0.0f;
    car->velocity.y = 0.0f;
    car->speed = 0.0f;
    car->acceleration = 0.0f;
    car->angular_velocity = 0.0f;
    car->furthest_point_index = 0;
    car->total_distance_traveled = 0.0f;
    car->time_alive = 0.0f;
    car->no_movement_frames = 0;
    car->is_alive = true;

    return car;
}

Car *destroy_car(Car *car)
{
    if (car != NULL) {
        free(car);
    }
    return NULL;
}

float get_car_speed(const Car *car)
{
    if (car == NULL) {
        fprintf(stderr, "Error: Car pointer is NULL in get_car_speed.\n");
        return 0.0f; // Return 0 speed for NULL car
    }
    return car->speed;
}

Point get_car_position(const Car *car)
{
    if (car == NULL) {
        fprintf(stderr, "Error: Car pointer is NULL in get_car_position.\n");
        return (Point){0.0f, 0.0f}; // Return origin for NULL car
    }
    return car->position;
}