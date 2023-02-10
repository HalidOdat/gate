@type vertex

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoords;

layout (std140) uniform Camera {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  mat4 projectionViewMatrix;
  vec4 position;
  vec4 front;
  vec4 clippingPlane;
} uCamera;

out vec2 vTexCoords;

void main() {
  vTexCoords     = aTexCoords;
  gl_Position    = vec4(aPosition, 0.0f, 1.0f);
}

@type fragment

precision mediump float;

out vec4 vFragmentColor;

in vec2 vTexCoords;
uniform sampler2D uScreenTexture;

void main() {
  vec3 color = vec3(texture(uScreenTexture, vTexCoords));

  // submit final color
  vFragmentColor = vec4(color, 1.0f);
}