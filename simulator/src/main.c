#include "window.h"
#include "shader.h"
#include <stdio.h>

int main(void) {
    if (window_init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE) == -1) {
        return 1;
    }

    const char* fragFile = "renderer/shaders/basic.frag";
    const char* vertexFile = "renderer/shaders/basic.vert";
    GLuint program = shader_create(vertexFile, fragFile);
    if (!program) {
        fprintf(stderr, "Shader Error");
        window_cleanup();
        return 1;
    }

    float verts[] = {-0.5, 0, 0, 0.5, 0.5, 0};

    GLuint vao = 0, vbo = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!window_should_close()) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shader_use(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        window_swap_and_poll();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    shader_clean_up(program);
    window_cleanup();

    return 0;
}