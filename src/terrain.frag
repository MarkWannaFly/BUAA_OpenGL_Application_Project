#version 450 core

in vec2 TexCoord;

layout(binding = 0) uniform sampler2D theTex;
layout(location = 0) out vec4 FragColor;

void main() {
    FragColor = texture(theTex, TexCoord);
}
