#version 450 core
in vec3 color;
layout (location = 0) out vec4 Frag_Color;

void main()
{
    // Frag_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    Frag_Color = vec4(color, 1.0f);
}