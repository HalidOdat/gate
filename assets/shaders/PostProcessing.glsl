#version 330 core

@type vertex

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoords;

out vec2 vTexCoords;

void main() {
    vTexCoords  = aTexCoords;
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0f, 1.0f);
}

@type fragment

in vec2 vTexCoords;

uniform sampler2D uScreenTexture;

out vec4 vFragmentColor;

void main() {
    vFragmentColor = texture(uScreenTexture, vTexCoords);
    float average = 0.2126 * vFragmentColor.r + 0.7152 * vFragmentColor.g + 0.0722 * vFragmentColor.b;
    vFragmentColor = vec4(average, average, average, 1.0);
}