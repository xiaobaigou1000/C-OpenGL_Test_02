#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec2 TexCoords;
uniform sampler2D tex;

void main()
{
    Frag_Color = texture(tex, TexCoords);
}