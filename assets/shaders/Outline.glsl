@type vertex

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexture;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in mat4 aModelMatrix;  // 3, 4, 5, 6
layout (location = 7) in mat3 aNormalMatrix; // 7, 8, 9
#if EDITOR
  layout (location = 10) in highp uint aEntityId;
#endif

layout (std140) uniform Camera {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  mat4 projectionViewMatrix;
  vec4 position;
  vec4 front;
} uCamera;

void main() {
  gl_Position = uCamera.projectionViewMatrix * aModelMatrix * vec4(aPosition, 1.0);
}

@type fragment

#if !GL_ES
  layout(early_fragment_tests) in;
#endif

precision mediump float;

out vec4 vFragmentColor;

void main() {
  vFragmentColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}
