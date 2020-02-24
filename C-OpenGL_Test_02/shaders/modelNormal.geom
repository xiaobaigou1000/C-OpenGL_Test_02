#version 450 core
layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in vec3 Normal[];

void genNormalLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(Normal[index], 0.0f) * 0.05f;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    genNormalLine(0);
    genNormalLine(1);
    genNormalLine(2);
}