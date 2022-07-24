#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec2 TexCoord;

uniform sampler2D uTexture;

uniform int invert = 0;

void main() {
   FragColor = vec4(invert, invert, invert, 0) + texture(uTexture, TexCoord) * vec4(Color, 1.0f);
}