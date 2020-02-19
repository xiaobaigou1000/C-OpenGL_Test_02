#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(position, 1.0f);
}