#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inColor;

out VS_OUT
{
    vec3 color;
}vs_out;

void main()
{
    vs_out.color = inColor;
    gl_Position = vec4(position, 1.0f);
}