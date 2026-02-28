#include "window.h"
#include <stdio.h>

static GLFWwindow* window;

int window_init(int width, int height, const char* title) {
    if (glfwInit() == GLFW_FALSE) {
        fprintf(stderr, "Window Initialization Failed");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Window Creation Failed");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "glad Loading Error");
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, width, height);

    return 0;
}

int window_should_close(void) {
    return glfwWindowShouldClose(window);
}

void window_swap_and_poll(void) {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void window_cleanup(void) {
    glfwDestroyWindow(window);
    glfwTerminate();
}