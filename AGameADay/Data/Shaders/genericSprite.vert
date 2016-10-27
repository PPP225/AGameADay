#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

uniform mat4 projection;
uniform mat4 model;

out vec2 TexCoords;

void main()
{
    gl_Position = projection * model * vec4(vertex.xy, 0.0f, 1.0f);
    TexCoords = vertex.zw;
}