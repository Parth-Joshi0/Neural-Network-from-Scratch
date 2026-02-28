#ifndef TRACK_RENDERER_H
#define TRACK_RENDERER_H

#include "track_internals.h"
#include "shader.h"
#include "glad.h"

int track_renderer_init(Boundary* left, Boundary* right);
void track_renderer_draw();
void track_renderer_cleanup();

#endif