#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;
layout (location = 4) in mat4 ModelMat;

out vec2 TexCoords;

uniform mat4 MV;

void main()
{
    gl_Position = MV * 
}