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

#ifndef THRESHOLD
  #define THRESHOLD 1.0f
#endif

float rgbToLuminance(vec3 color) {
  return dot(color, vec3(0.2126f, 0.7152f, 0.0722f));
}

void main() { 
  vec3 color = vec3(texture(uScreenTexture, vTexCoords));
  if (rgbToLuminance(color) > THRESHOLD) {
    vFragmentColor = vec4(color, 1.f);
  } else {
    vFragmentColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  }
}