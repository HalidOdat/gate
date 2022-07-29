#version 330 core

@type vertex

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexture;
layout (location = 2) in vec3 aNormal;

out vec2 vTexture;

uniform mat4 uProjectionView;
// uniform mat4 uTransform;

void main() {
  vTexture = aTexture;
  gl_Position = uProjectionView * vec4(aPosition, 1.0);
}

@type fragment

out vec4 vFragmentColor;

in vec2 vTexture;

uniform sampler2D uTexture;

void main() {
  vFragmentColor = texture(uTexture, vTexture);
}