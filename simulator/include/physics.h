#ifndef PHYSICS_H
#define PHYSICS_H

#include "car.h"

void update_car_physics(Car *car, float acceleration, float steering_angle, const float dt);

#endif