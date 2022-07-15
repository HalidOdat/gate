#version 330 core
out vec4 FragColor;

uniform vec4 uColor = vec4(1.0f);

void main() {
   FragColor = uColor;
}