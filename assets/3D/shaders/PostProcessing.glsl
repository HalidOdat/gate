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
out vec2 vPosition;
flat out vec3 vViewPosition;
flat out vec2 vClippingPlane;

void main() {
  vTexCoords     = aTexCoords;
  vPosition      = aPosition;
  vViewPosition  = vec3(uCamera.position);
  vClippingPlane = vec2(uCamera.clippingPlane);
  gl_Position    = vec4(aPosition, 0.0f, 1.0f);
}

@type fragment

precision mediump float;

in vec2 vPosition;
in vec2 vTexCoords;
flat in vec3 vViewPosition;
flat in vec2 vClippingPlane;

uniform sampler2D uScreenTexture;
uniform sampler2D uDepthMap;

out vec4 vFragmentColor;

vec3 aces(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

#ifndef FOG
  #define FOG 0
#endif

void main() {
  const float exposure   = 1.0f;
  const float contrast   = 1.0f;
  const float brightness = 0.0f;
  const float saturation = 1.0f;
  const float fogDensity = 0.008f;

  vec3 color = vec3(texture(uScreenTexture, vTexCoords));

  #if FOG
    /// Fog
    float depth = texture(uDepthMap, vTexCoords).x;
    float viewDistance = (1.0f - vClippingPlane.y / vClippingPlane.x) * depth + vClippingPlane.y / vClippingPlane.x;
    viewDistance       = 1.0f / viewDistance;
    viewDistance       *= vClippingPlane.y;
    
    float fogFactor = pow(2.0f, -((viewDistance*fogDensity)*(viewDistance*fogDensity)));
    
    color = mix(vec3(0.5f), color, fogFactor);
  #endif


  // exposure
  // color = color * exposure;

  // contrast & brighness
  // color = contrast * (color - 0.5f) + 0.5f + brightness;

  // saturation
  // float grayscale = dot(color, vec3(0.299f, 0.587f, 0.114f));
  // color = mix(vec3(grayscale), color, saturation);

  // tone-mapping
  // color = aces(color);

  // gamma correction
  // const float gamma = 2.2;
  // color = pow(color, vec3(1.0f/gamma));

  // submit final color
  vFragmentColor = vec4(color, 1.0f);
}