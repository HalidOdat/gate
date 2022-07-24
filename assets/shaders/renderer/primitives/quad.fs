#version 330 core
out vec4 FragColor;

in vec4 vColor;
in vec2 vTexCoord;
flat in uint  vTexIndex;

uniform sampler2D uTextures[32];

void main() {
   FragColor = texture(uTextures[vTexIndex], vTexCoord) * vColor;
}
