#version 450 core
layout (location = 0) out vec4 Frag_Color;

in vec2 TexCoords;
uniform sampler2D tex;

void main()
{
    vec4 color = texture(tex, TexCoords);
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    Frag_Color = vec4(average, average, average, 1.0f);
}