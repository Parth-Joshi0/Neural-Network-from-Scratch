#include "track_renderer.h"
#include <stdio.h>

static GLuint leftVAO = 0, leftVBO = 0;
static GLuint rightVAO = 0, rightVBO = 0;
static int left_points = 0;
static int right_points = 0;
static GLuint shader = 0;
static float transformation_matrix[16];

void create_transformation_matrix(float M[16], float l, float r, float b, float t) {
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

int track_renderer_init(Boundary* left, Boundary* right) {
    create_transformation_matrix(transformation_matrix, 0, 100, 0, 100);

    left_points = left->count;
    right_points = right->count;

    const char* fragFile = "renderer/shaders/basic.frag";
    const char* vertexFile = "renderer/shaders/basic.vert";
    shader = shader_create(vertexFile, fragFile);
    if (!shader) {
        fprintf(stderr, "Shader Error");
        return 1;
    }
    glGenVertexArrays(1, &leftVAO);
    glBindVertexArray(leftVAO);

    glGenBuffers(1, &leftVBO);
    glBindBuffer(GL_ARRAY_BUFFER, leftVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * left_points * 2, left->points, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &rightVAO);
    glBindVertexArray(rightVAO);

    glGenBuffers(1, &rightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rightVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * right_points * 2, right->points, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  
    return 0;
}

void track_renderer_draw() {
    shader_use(shader);

    GLint projLoc = shader_get_uniform(shader, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, transformation_matrix);

    GLint colourLoc = shader_get_uniform(shader, "colour");
    glUniform3f(colourLoc, 1.0f, 1.0f, 1.0f);

    glBindVertexArray(leftVAO);
    glDrawArrays(GL_LINE_STRIP, 0, left_points);

    glBindVertexArray(rightVAO);
    glDrawArrays(GL_LINE_STRIP, 0, right_points);

}

void track_renderer_cleanup() {
    shader_clean_up(shader);
    glDeleteVertexArrays(1, &leftVAO);
    glDeleteBuffers(1, &leftVBO);
    glDeleteVertexArrays(1, &rightVAO);
    glDeleteBuffers(1, &rightVBO);
}

