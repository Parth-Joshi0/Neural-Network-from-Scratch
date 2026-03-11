#include "ray_renderer.h"
#include <stdio.h>
#include <math.h>
#include "util.h"
#include "ray_cast.h"

static GLuint vao = 0, vbo = 0;
static GLuint shader = 0;
static float transformation_matrix[16];
static float ray_array[4 * NUM_RAYS];

int ray_renderer_init() {
    create_transformation_matrix(transformation_matrix, 0, 100, 0, 100);

    const char* fragFile = "renderer/shaders/basic.frag";
    const char* vertexFile = "renderer/shaders/basic.vert";
    shader = shader_create(vertexFile, fragFile);
    if (!shader) {
        fprintf(stderr, "Shader Error");
        return 1;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * NUM_RAYS, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return 0;
}

void ray_renderer_update(Car* car) {

    for (int j = 0; j < NUM_RAYS; j++) {
        float end_x = car->position.x + car->ray_distances[j] * cosf(car->heading + RAY_ANGLES[j]);
        float end_y = car->position.y + car->ray_distances[j] * sinf(car->heading + RAY_ANGLES[j]);
        ray_array[j*4] = car->position.x;
        ray_array[j*4 + 1] = car->position.y;
        ray_array[j*4 + 2] = end_x;
        ray_array[j*4 + 3] = end_y;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * NUM_RAYS * 4, ray_array);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ray_renderer_draw() {
    shader_use(shader);

    GLint projLoc = shader_get_uniform(shader, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, transformation_matrix);

    GLint colourLoc = shader_get_uniform(shader, "colour");
    glUniform3f(colourLoc, 1.0f, 0.0f, 0.0f);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, NUM_RAYS * 2);
}

void ray_renderer_cleanup() {
    shader_clean_up(shader);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}