#include "track_renderer.h"
#include <stdio.h>
#include "util.h"

static GLuint leftVAO = 0, leftVBO = 0;
static GLuint rightVAO = 0, rightVBO = 0;
static GLuint startVAO = 0, startVBO = 0;
static int left_points = 0;
static int right_points = 0;
static GLuint shader = 0;
static float transformation_matrix[16];

int track_renderer_init(Boundary* left, Boundary* right, struct BoundarySegment* start_seg) {
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

    // Start boundary segment (two points: start and end)
    float start_pts[4] = {
        start_seg->start.x, start_seg->start.y,
        start_seg->end.x,   start_seg->end.y
    };

    glGenVertexArrays(1, &startVAO);
    glBindVertexArray(startVAO);

    glGenBuffers(1, &startVBO);
    glBindBuffer(GL_ARRAY_BUFFER, startVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(start_pts), start_pts, GL_STATIC_DRAW);

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

    // Draw start boundary in red
    glUniform3f(colourLoc, 1.0f, 0.0f, 0.0f);
    glBindVertexArray(startVAO);
    glDrawArrays(GL_LINES, 0, 2);

}

void track_renderer_cleanup() {
    shader_clean_up(shader);
    glDeleteVertexArrays(1, &leftVAO);
    glDeleteBuffers(1, &leftVBO);
    glDeleteVertexArrays(1, &rightVAO);
    glDeleteBuffers(1, &rightVBO);
    glDeleteVertexArrays(1, &startVAO);
    glDeleteBuffers(1, &startVBO);
}

