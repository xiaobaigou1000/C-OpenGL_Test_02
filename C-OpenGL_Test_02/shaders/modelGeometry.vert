#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 inTexCoords;

out VS_OUT
{
    vec2 TexCoords;
}vs_out;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(position, 1.0f);
    vs_out.TexCoords = inTexCoords;
}