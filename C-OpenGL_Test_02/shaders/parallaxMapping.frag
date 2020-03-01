#version 450 core
layout (location = 0) out vec4 Frag_Color;

in VS_OUT
{
    vec3 FragPos;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    mat3 TBN;
    vec3 TangentFragPos;
    vec3 TangentViewPos;
    vec3 TangentLightPos;
}fs_in;

uniform sampler2D tex;
uniform sampler2D shadowMap;
uniform sampler2D normalMap;
uniform sampler2D displacementMap;
uniform float heightScale;


float shadowCaculation(vec4 lightSpaceFragPos)
{
    vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    float bias = 0.005;
    vec2 texPixelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texPixelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height = texture(displacementMap, texCoords).r;
    vec2 p = viewDir.xy * (height * heightScale);
    return texCoords - p;
}

void main()
{
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = parallaxMapping(fs_in.TexCoords, viewDir);
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    vec3 color = texture(tex, texCoords).rgb;
    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - vec3(1.0));
    vec3 lightColor = vec3(1.0f);

    vec3 ambientStrength= 0.15 * lightColor;

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuseStrength = diff * lightColor;

    vec3 halfwayDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfwayDir, normal), 0.0), 64.0);
    vec3 specularStrength = spec * lightColor;

    float shadow = shadowCaculation(fs_in.FragPosLightSpace);
    vec3 result = (ambientStrength + (1.0 - shadow) * (diffuseStrength + specularStrength)) * color;
    Frag_Color = vec4(result, 1.0);
}