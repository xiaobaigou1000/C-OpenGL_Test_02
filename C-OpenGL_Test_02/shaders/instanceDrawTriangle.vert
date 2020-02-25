#version 450 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 inColor;

out vec3 color;

uniform vec2 offsets[100];

void main()
{
    color = inColor;
    gl_Position = vec4(position + offsets[gl_InstanceID], 1.0, 1.0);
}