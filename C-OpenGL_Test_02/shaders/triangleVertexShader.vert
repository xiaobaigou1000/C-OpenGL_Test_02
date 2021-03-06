#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoords;
out vec3 color;
out vec2 TexCoords;

uniform mat4 translateMat;

void main()
{
    color = inColor;
    TexCoords = inTexCoords;
    gl_Position = translateMat * vec4(position, 1.0f);
}