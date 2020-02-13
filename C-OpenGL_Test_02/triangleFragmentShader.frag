#version 450 core
uniform vec4 color;
layout (location = 0) out vec4 Frag_Color;

void main()
{
    // Frag_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    Frag_Color = color;
}