#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
}vs_out;

uniform mat4 VP;

void main()
{
    gl_Position = VP * vec4(position, 1.0);
    vs_out.FragPos = position;
    vs_out.Normal = inNormal;
    vs_out.TexCoords = inTexCoords;
}