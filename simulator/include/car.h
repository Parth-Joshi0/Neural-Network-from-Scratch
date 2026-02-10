#ifndef CAR_H
#define CAR_H

#include "types.h"

typedef struct Car Car;

Car *create_car(Point start_position, float start_direction);
Car *reset_car(Car *car, Point start_position, float start_direction);
Car *destroy_car(Car *car);

float get_car_speed(const Car *car);
Point get_car_position(const Car *car);
#endif