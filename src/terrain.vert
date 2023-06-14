#version 450 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoord = VertexTexCoord;
    gl_Position = projection * view * model * vec4(VertexPosition, 1.0);
}
