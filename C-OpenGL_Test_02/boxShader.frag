#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

uniform sampler2D tex;
uniform vec3 viewPos;

float ambientStrength = 0.1f;
float specularStrength = 0.5f;

void main()
{
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f),material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;
    Frag_Color = vec4(result, 1.0f);
}