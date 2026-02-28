#ifndef SHADER_H
#define SHADER_H

#include "glad.h"

GLuint shader_create(const char* vertex_file_path, const char* fragment_file_path);
void shader_use(GLuint program_ID);
void shader_clean_up(GLuint program_ID);

#endif