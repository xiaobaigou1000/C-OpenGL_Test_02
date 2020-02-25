#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec3 color;

void main()
{
    Frag_Color = vec4(color, 1.0);
}