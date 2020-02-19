#version 450 core
layout (location = 0) out vec4 Frag_Color;

uniform vec3 lightColor;

void main()
{
    Frag_Color = vec4(lightColor, 1.0f);
}