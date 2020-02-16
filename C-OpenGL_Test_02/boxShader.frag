#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D tex;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    // float diff = dot(norm, lightDir);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    Frag_Color = vec4(result, 1.0f);
}