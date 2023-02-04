@type vertex

layout (location = 0) in vec3 aPosition;

out vec3 vTexCoords;

uniform mat4 uProjection;
uniform mat4 uView;

void main() {
  vTexCoords = aPosition;
  vec4 position = uProjection * uView * vec4(aPosition, 1.0);
  gl_Position = position.xyww;
}  

@type fragment

precision mediump float;

out vec4 vFragmentColor;

in vec3 vTexCoords;

uniform samplerCube uSkybox;

void main() {
  #ifdef COLOR
    vFragmentColor = COLOR;
  #else
    vFragmentColor = texture(uSkybox, vTexCoords);
  #endif
}

