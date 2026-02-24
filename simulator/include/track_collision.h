#ifndef TRACK_COLLISION_H
#define TRACK_COLLISION_H

#include "quad_tree.h"
#include "car_internals.h"
#include "car.h"

int check_car_collision(Car* car, QuadTreeNode* node);
void get_corners(Car* car, Point* corners);

#define MAX_COLLISION_CHECKS 128

#endif