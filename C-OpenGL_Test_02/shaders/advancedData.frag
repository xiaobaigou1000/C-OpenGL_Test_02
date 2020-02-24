#version 450 core
layout (location = 0) out vec4 Frag_Color;

in VS_OUT
{
    vec3 Normal;
    vec2 TexCoords;
}vs_in;

void main()
{
    Frag_Color = vec4(1.0f);
}