#ifndef RAY_RENDERER_H
#define RAY_RENDERER_H

#include "car_internals.h"
#include "shader.h"
#include "glad.h"

int ray_renderer_init();
void ray_renderer_update(Car* car);
void ray_renderer_draw();
void ray_renderer_cleanup();

#endif