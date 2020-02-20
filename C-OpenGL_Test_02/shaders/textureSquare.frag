#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec2 TexCoords;
uniform sampler2D tex;

void main()
{
    vec4 color = texture(tex, TexCoords);
    if(color.a < 0.1f)
        discard;
    Frag_Color = color;
}