#ifndef WINDOW_H
#define WINDOW_H

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Neural Network Simulator"

#include "glad.h"
#include <GLFW/glfw3.h>

int window_init(int width, int height, const char* title);
int window_should_close(void);
void window_swap_and_poll(void);
void window_cleanup(void);

#endif