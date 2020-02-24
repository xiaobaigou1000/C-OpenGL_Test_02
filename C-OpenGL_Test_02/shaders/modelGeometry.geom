#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec2 TexCoords;
}gs_in[];

out vec2 TexCoords;

uniform float explodeConst;

vec3 getNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
    vec3 normal = normalize(cross(a, b));
    return normal;
}

vec4 explode(vec4 position, vec3 normal)
{
    return position + vec4(explodeConst * normal, 1.0f);
}

void main()
{
    vec3 normal = getNormal();
    gl_Position = explode(gl_in[0].gl_Position, normal);
    TexCoords = gs_in[0].TexCoords;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    TexCoords = gs_in[1].TexCoords;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    TexCoords = gs_in[2].TexCoords;
    EmitVertex();
}