#version 450 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 offset;

out vec3 color;

void main()
{
    vec2 pos = position * gl_InstanceID / 100.0;
    color = inColor;
    gl_Position = vec4(pos + offset, 1.0, 1.0);
}