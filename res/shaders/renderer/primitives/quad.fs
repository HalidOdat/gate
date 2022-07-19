#version 330 core
out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D uTexture;
uniform vec4      uColor = vec4(1.0f);

void main() {
   FragColor = texture(uTexture, vTexCoord) * uColor;
}
