#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT
{
    vec3 color;
}gs_in[];

out vec3 color;

void build_house(vec4 position)
{
    gl_Position = position + vec4(-0.1, -0.1, 0.0, 0.0);    // 1:bottom-left
    color = gs_in[0].color - vec3(0.1f, 0.1f, 0.1f);
    EmitVertex();   
    gl_Position = position + vec4( 0.1, -0.1, 0.0, 0.0);    // 2:bottom-right
    color = gs_in[0].color - vec3(0.1f, 0.1f, 0.1f);
    EmitVertex();
    gl_Position = position + vec4(-0.1,  0.1, 0.0, 0.0);    // 3:top-left
    EmitVertex();
    gl_Position = position + vec4( 0.1,  0.1, 0.0, 0.0);    // 4:top-right
    EmitVertex();
    gl_Position = position + vec4( 0.0,  0.2, 0.0, 0.0);    // 5:top
    color = gs_in[0].color + vec3(0.1f, 0.1f, 0.1f);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    color = gs_in[0].color;
    build_house(gl_in[0].gl_Position);
}