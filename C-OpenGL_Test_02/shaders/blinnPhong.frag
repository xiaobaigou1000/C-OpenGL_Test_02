#version 450 core
layout (location = 0) out vec4 Frag_Color;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
}fs_in;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D floorTex;

void main()
{
    vec3 color = texture(floorTex, fs_in.TexCoords).rgb;
    vec3 ambient = 0.05 * color;
    
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff =  max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.3) * spec;

    Frag_Color = vec4(ambient + diffuse + specular, 1.0);
}