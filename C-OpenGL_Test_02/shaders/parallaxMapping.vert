#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;
layout (location = 3) in mat4 modelMat;
layout (location = 7) in vec3 inTangent;
layout (location = 8) in vec3 inBitangent;

uniform mat4 VP;
uniform mat4 lightSpaceVO;

uniform vec3 lightPos;
uniform vec3 viewPos;

out VS_OUT
{
    vec3 FragPos;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    mat3 TBN;
    vec3 TangentFragPos;
    vec3 TangentViewPos;
    vec3 TangentLightPos;
}vs_out;

void main()
{
    gl_Position = VP * modelMat * vec4(position, 1.0f);
    mat3 normalFixMat = transpose(inverse(mat3(modelMat)));
    vec3 N = normalize(normalFixMat * inNormal);
    vec3 T = normalize(normalFixMat * inTangent);
    vec3 B = normalize(normalFixMat * inBitangent);
    vs_out.TBN = transpose(mat3(T, B, N));
    vs_out.TexCoords = inTexCoords;
    vs_out.FragPos = vec3(modelMat * vec4(position, 1.0f));
    vs_out.FragPosLightSpace = lightSpaceVO * vec4(vs_out.FragPos, 1.0);
    vs_out.TangentFragPos = vs_out.TBN * vs_out.FragPos;
    vs_out.TangentLightPos = vs_out.TBN * lightPos;
    vs_out.TangentViewPos = vs_out.TBN * viewPos;
}