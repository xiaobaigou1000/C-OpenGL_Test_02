#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec2 TexCoords;
uniform sampler2D tex;

const float offset = 1.0 / 500.0;
const vec2 offsets[9] = vec2[](
    vec2(-offset,  offset), // top-left
    vec2( 0.0f,    offset), // top-center
    vec2( offset,  offset), // top-right
    vec2(-offset,  0.0f),   // center-left
    vec2( 0.0f,    0.0f),   // center-center
    vec2( offset,  0.0f),   // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f,   -offset), // bottom-center
    vec2( offset, -offset)  // bottom-right    
);

float kernel[9] = float[](
    1, 1, 1,
    1,-8, 1,
    1, 1, 1
);

void main()
{
    vec3 color = vec3(0.0f);
    for(int i = 0; i < 9; ++i)
    {
        color += vec3(texture(tex, TexCoords + offsets[i])) * kernel[i];
    }
    Frag_Color = vec4(color, 1.0f);
}