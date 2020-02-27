#version 450 core
layout (location = 0) in vec3 position;
layout (location = 3) in mat4 modelMat;

uniform mat4 lightVP;

void main()
{
    gl_Position = lightVP * modelMat * vec4(position, 1.0);
}