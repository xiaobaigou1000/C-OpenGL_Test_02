#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec3 TexCoords;
uniform samplerCube tex;

void main()
{
    Frag_Color = texture(tex, TexCoords);
}