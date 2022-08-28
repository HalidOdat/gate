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

precision mediump float;

in vec2 vPosition;
in vec2 vTexCoords;

uniform sampler2D uScreenTexture;

out vec4 vFragmentColor;

#ifndef WEB_GL
  #define HARDWARE_GAMMA_CORRECTION 1
#endif

vec3 aces(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
  const float exposure   = 1.0f;
  const float contrast   = 1.0f;
  const float brightness  = 0.0f;
  const float saturation = 1.0f;

  vec3 color = vec3(texture(uScreenTexture, vTexCoords));

  // exposure
  color = color * exposure;

  // contrast & brighness
  color = contrast * (color - 0.5f) + 0.5f + brightness;

  // saturation
  float grayscale = dot(color, vec3(0.299f, 0.587f, 0.114f));
  color = mix(vec3(grayscale), color, saturation);

  // tone-mapping
  color = aces(color);

  // gamma correction
  #ifndef HARDWARE_GAMMA_CORRECTION
    const float gamma = 2.2;
    color = pow(color, vec3(1.0f/gamma)), 1.0f;
  #endif

  // submit final color
  vFragmentColor = vec4(color, 1.0f);
}