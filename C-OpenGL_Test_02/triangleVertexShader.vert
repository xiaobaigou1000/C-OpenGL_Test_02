#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoords;
out vec3 color;
out vec2 TexCoords;

void main()
{
    color = inColor;
    TexCoords = inTexCoords;
    gl_Position = vec4(position, 1.0f);
}