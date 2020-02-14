#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 inTexCoords;
out vec2 TexCoords;

uniform mat4 MVP;

void main()
{
    TexCoords = inTexCoords;
    gl_Position = MVP * vec4(position,1.0f);
}