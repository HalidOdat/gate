#version 330 core

@type vertex

layout (location = 0) in vec3 aPos;

void main() {
   gl_Position = vec4(aPos, 1.0);
}

@type fragment

out vec4 FragColor;

uniform vec4 uColor = vec4(1.0f);

void main() {
   FragColor = uColor;
}