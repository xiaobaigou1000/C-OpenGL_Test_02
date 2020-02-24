#version 450 core
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

out VS_OUT
{
    vec3 Normal;
    vec2 TexCoords;
}vs_out;

layout (std140) uniform Matrices
{
    mat4 VP;
};

uniform mat4 modelMat;

void main()
{
    gl_Position = VPvec4(position, 1.0f);
}