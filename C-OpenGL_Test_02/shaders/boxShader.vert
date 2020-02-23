#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 inNormal;
layout (location = 2) in vec3 inTexCoords;
out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 MVP;
uniform mat4 modelMat;

void main()
{
    TexCoords = inTexCoords;
    Normal = mat3(transpose(inverse(modelMat))) * inNormal;
    FragPos = (modelMat * vec4(position, 1.0f)).xyz;
    gl_Position = MVP * vec4(position, 1.0f);
}