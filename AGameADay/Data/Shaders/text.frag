#version 330 core
in vec2 TexCoords;

uniform sampler2D texture0;
uniform vec3 textColor;

out vec4 color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(texture0, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}