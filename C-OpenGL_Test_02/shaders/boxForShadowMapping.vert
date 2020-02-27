#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;
layout (location = 3) in mat4 modelMat;

uniform mat4 VP;
uniform mat4 lightSpaceVO;

out VS_OUT
{
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
}vs_out;

void main()
{
    gl_Position = VP * modelMat * vec4(position, 1.0f);
    vs_out.Normal = transpose(inverse(mat3(modelMat))) * inNormal;
    vs_out.TexCoords = inTexCoords;
    vs_out.FragPos = vec3(modelMat * vec4(position, 1.0f));
    vs_out.FragPosLightSpace = lightSpaceVO * vec4(vs_out.FragPos, 1.0);
}