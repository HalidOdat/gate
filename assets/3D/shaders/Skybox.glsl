@type vertex

layout (location = 0) in vec3 aPosition;

out vec3 vTexCoords;

uniform mat4 uProjection;
uniform mat4 uView;
uniform vec4 uInvalidEntity;

flat out vec4 vInvalidEntity;

void main() {
  vInvalidEntity = uInvalidEntity;
  vTexCoords = aPosition;
  vec4 position = uProjection * uView * vec4(aPosition, 1.0);
  gl_Position = position.xyww;
}

@type fragment

precision mediump float;
precision highp int;

layout (location = 0) out vec4 vFragmentColor;
layout (location = 1) out vec4 vEntityIdOut;

in vec3 vTexCoords;

uniform samplerCube uSkybox;

flat in vec4 vInvalidEntity;
// #define INVALID_ENTITY vec4(1.0f, 1.0f, 1.0f);

void main() {
  #ifdef COLOR
    vFragmentColor = COLOR;
  #else
    vFragmentColor = texture(uSkybox, vTexCoords);
  #endif

  vEntityIdOut = vInvalidEntity;
}

