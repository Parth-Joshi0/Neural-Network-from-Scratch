#include "window.h"
#include <stdio.h>

static GLFWwindow* window;
static void framebuffer_size_callback(GLFWwindow* w, int width, int height);

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

    glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

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

static void framebuffer_size_callback(GLFWwindow* w, int width, int height) {
    (void) w;
    glViewport(0, 0, width, height);
}