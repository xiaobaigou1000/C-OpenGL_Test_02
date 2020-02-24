#version 450 core
layout (location = 0) out vec4 Frag_Color;

uniform vec4 color;

void main()
{
    Frag_Color = color;
}