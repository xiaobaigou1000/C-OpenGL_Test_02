#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec3 Normal;
in vec3 FragPos;

uniform samplerCube cubeMap;
uniform vec3 viewPos;

void main()
{
    float ratio = 1.0 / 1.52;
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    Frag_Color = vec4(texture(cubeMap, R));
}