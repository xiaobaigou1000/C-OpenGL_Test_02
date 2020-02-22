#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec3 Normal;
in vec3 FragPos;

uniform samplerCube cubeMap;
uniform vec3 viewPos;

void main()
{
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(Normal));
    Frag_Color = vec4(texture(cubeMap, R));
}