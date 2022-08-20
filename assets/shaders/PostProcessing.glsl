#version 300 es

precision mediump float;
precision lowp sampler2D;
precision lowp samplerCube;

@type vertex

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoords;

out vec2 vTexCoords;
out vec2 vPosition;

void main() {
  vTexCoords  = aTexCoords;
  vPosition   = aPosition;
  gl_Position = vec4(aPosition, 0.0f, 1.0f);
}

@type fragment

in vec2 vPosition;
in vec2 vTexCoords;

uniform sampler2D uScreenTexture;

out vec4 vFragmentColor;

void main() {
  vFragmentColor = texture(uScreenTexture, vTexCoords);
  // if (vPosition.x < 0.0f) {
  //   float average = 0.2126 * vFragmentColor.r + 0.7152 * vFragmentColor.g + 0.0722 * vFragmentColor.b;
  //   vFragmentColor = vec4(average, average, average, 1.0);
  // }
}