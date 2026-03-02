#version 330 core

layout (location = 0) in vec2 vertexPos;
layout (location = 1) in vec2 instancePos;
layout (location = 2) in float heading;

uniform mat4 projection;

void main() {
    float c = cos(heading);
    float s = sin(heading);
    vec2 rotated = vec2(
        vertexPos.x * c - vertexPos.y * s,
        vertexPos.x * s + vertexPos.y * c
    );
    gl_Position = projection * vec4(rotated + instancePos, 0, 1.0);
}