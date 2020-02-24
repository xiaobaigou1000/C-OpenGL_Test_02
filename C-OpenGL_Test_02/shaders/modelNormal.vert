#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

out vec3 Normal;

uniform mat4 MVP;
uniform mat3 normalMat;

void main()
{
    gl_Position = MVP * vec4(position, 1.0f);
    Normal = normalize(normalMat * inNormal);
}