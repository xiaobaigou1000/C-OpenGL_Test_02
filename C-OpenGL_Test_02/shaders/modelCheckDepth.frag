#version 450 core
layout (location = 0) out vec4 Frag_Color;

float near = 0.1f;
float far = 5.0f;
float gamma = 2.2f;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{

    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    Frag_Color = vec4(pow(vec3(depth), vec3(1.0f / gamma)), 1.0);
}