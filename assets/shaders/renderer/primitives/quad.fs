#version 330 core
out vec4 FragColor;

in vec4 vColor;
in vec2 vTexCoord;

uniform sampler2D uTexture;

void main() {
   FragColor = texture(uTexture, vTexCoord) * vColor;
}
