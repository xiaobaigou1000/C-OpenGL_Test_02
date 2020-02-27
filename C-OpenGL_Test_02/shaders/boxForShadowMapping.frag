#version 450 core
layout (location = 0) out vec4 Frag_Color;

in VS_OUT
{
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
}fs_in;

uniform sampler2D tex;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float shadowCaculation(vec4 lightSpaceFragPos)
{
    vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    vec3 color = texture(tex, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0f);

    vec3 ambientStrength= 0.15 * lightColor;

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuseStrength = diff * lightColor;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfwayDir, normal), 0.0), 64.0);
    vec3 specularStrength = spec * lightColor;

    float shadow = shadowCaculation(fs_in.FragPosLightSpace);
    vec3 result = (ambientStrength + (1.0 - shadow) * (diffuseStrength + specularStrength)) * color;
    Frag_Color = vec4(result, 1.0);
}