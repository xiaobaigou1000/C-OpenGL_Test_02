#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec2 TexCoords;
uniform sampler2D tex;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    Frag_Color = vec4(lightColor * objectColor,1.0f);
}