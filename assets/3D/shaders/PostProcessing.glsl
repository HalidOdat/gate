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

vec3 texture2DAA(sampler2D tex, vec2 uv) {
  vec2 texsize = vec2(textureSize(tex, 0));
  vec2 uv_texspace = uv * texsize;
  vec2 seam = floor(uv_texspace + 0.5f);
  uv_texspace = (uv_texspace - seam) / fwidth(uv_texspace) + seam;
  uv_texspace = clamp(uv_texspace, seam - 0.5f, seam + 0.5f);
  return vec3(texture(tex, uv_texspace / texsize));
}

void main() {
  vec3 color = texture2DAA(uScreenTexture, vTexCoords);
  
  // submit final color
  vFragmentColor = vec4(color, 1.0f);
}
