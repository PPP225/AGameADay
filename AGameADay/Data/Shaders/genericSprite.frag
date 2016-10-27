#version 330 core
in vec2 TexCoords;

uniform sampler2D texture0;
uniform vec4 spriteColor;

out vec4 color;

void main()
{
    color = spriteColor * texture(texture0, TexCoords);
}