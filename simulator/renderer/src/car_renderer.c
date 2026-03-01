#include "car_renderer.h"
#include <stdio.h>

static GLuint vao = 0, rectVBO = 0, dynaVBO;
static GLuint shader = 0;
static float transformation_matrix[16];
static int num_cars = 0;

static void create_transformation_matrix(float M[16], float l, float r, float b, float t) {
    M[0]  =  2.0f / (r - l);
    M[1]  =  0.0f;
    M[2]  =  0.0f;
    M[3]  =  0.0f;

    M[4]  =  0.0f;
    M[5]  =  2.0f / (t - b);
    M[6]  =  0.0f;
    M[7]  =  0.0f;

    M[8]  =  0.0f;
    M[9]  =  0.0f;
    M[10] = -1.0f;
    M[11] =  0.0f;

    M[12] = -(r + l) / (r - l);
    M[13] = -(t + b) / (t - b);
    M[14] =  0.0f;
    M[15] =  1.0f;
}

int car_renderer_init(int num) {
    create_transformation_matrix(transformation_matrix, 0, 100, 0, 100);

    num_cars = num;

    const char* fragFile = "renderer/shaders/car.frag";
    const char* vertexFile = "renderer/shaders/car.vert";
    shader = shader_create(vertexFile, fragFile);
    if (!shader) {
        fprintf(stderr, "Shader Error");
        return 1;
    }

    float rectVerts[8] = {-CAR_HALF_WIDTH, CAR_HALF_LENGTH, CAR_HALF_WIDTH, CAR_HALF_LENGTH, CAR_HALF_WIDTH, -CAR_HALF_LENGTH, -CAR_HALF_WIDTH, -CAR_HALF_LENGTH};

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &rectVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, rectVerts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &dynaVBO);
    glBindBuffer(GL_ARRAY_BUFFER, dynaVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_cars * 3, NULL, GL_DYNAMIC_DRAW);

    // Location 1 — instance position (x, y)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // Location 2 — heading (after 2 floats)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return 0;
}

void car_renderer_update(int num, Car** cars) {
    num_cars = num;

    float car_data[num * 3];
    for (int i = 0; i < num; i++) {
        car_data[i * 3 + 0] = cars[i]->position.x;
        car_data[i * 3 + 1] = cars[i]->position.y;
        car_data[i * 3 + 2] = cars[i]->heading;
    }

    glBindBuffer(GL_ARRAY_BUFFER, dynaVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * num_cars * 3, car_data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void car_renderer_draw() {
    shader_use(shader);

    GLint projLoc = shader_get_uniform(shader, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, transformation_matrix);

    GLint colourLoc = shader_get_uniform(shader, "colour");
    glUniform3f(colourLoc, 0.0f, 1.0f, 1.0f);

    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, num_cars);
}

void car_renderer_cleanup() {
    shader_clean_up(shader);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &rectVBO);
    glDeleteBuffers(1, &dynaVBO);
}