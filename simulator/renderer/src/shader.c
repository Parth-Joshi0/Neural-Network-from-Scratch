#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

GLuint create(const char* file_path, GLenum type) {
    FILE* f = fopen(file_path, "rb");
    if (!f) {
        fprintf(stderr, "Could not find file: %s", file_path);
        return 0;
    }

    // go to end to get size
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    // allocate buffer (+1 for null terminator)
    char* buffer = malloc(size + 1);
    if (!buffer) {
        fclose(f);
        fprintf(stderr, "Malloc Failed Reader shader file: %s", file_path);
        return 0;
    }

    // read file
    fread(buffer, 1, size, f);
    buffer[size] = '\0';

    fclose(f);

    fflush(stdout);
    GLuint shaderID = glCreateShader(type);
    fflush(stdout);
    const char* src = buffer;
    glShaderSource(shaderID, 1, &src, NULL);
    free(buffer);
    glCompileShader(shaderID);

    GLint status = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (!status) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        fprintf(stderr, "Shader Error: %s", infoLog);
        glDeleteShader(shaderID);
        return 0;
    }

    return shaderID;
}

GLuint shader_create(const char* vertex_file_path, const char* fragment_file_path) {
    GLuint vertexShader = create(vertex_file_path, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        fprintf(stderr, "Vertex Shader Issue");
        return 0;
    }
    GLuint fragmentShader = create(fragment_file_path, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        fprintf(stderr, "Fragment Shader Issue");
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        char* log = (char*)malloc(len);
        glGetProgramInfoLog(program, len, NULL, log);
        fprintf(stderr, "Program link error:\n%s\n", log);
        free(log);
        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void shader_use(GLuint program_ID) {
    glUseProgram(program_ID);
}

void shader_clean_up(GLuint program_ID) {
    glDeleteProgram(program_ID);
}
