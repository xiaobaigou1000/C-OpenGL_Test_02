#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
    Frag_Color = texture(texture_diffuse1, TexCoords);
}