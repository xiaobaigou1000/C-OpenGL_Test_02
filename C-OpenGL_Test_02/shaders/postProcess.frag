#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec2 TexCoords;
uniform sampler2D tex;

void main()
{
    Frag_Color = vec4(1.0f, 1.0f, 1.0f, 1.0f) - texture(tex, TexCoords);
}