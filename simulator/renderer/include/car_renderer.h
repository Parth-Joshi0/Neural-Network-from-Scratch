#ifndef CAR_RENDERER_H
#define CAR_RENDERER_H

#include "car_internals.h"
#include "shader.h"
#include "glad.h"

int car_renderer_init(int num);
void car_renderer_update(int num, Car** cars);
void car_renderer_draw();
void car_renderer_cleanup();

#endif