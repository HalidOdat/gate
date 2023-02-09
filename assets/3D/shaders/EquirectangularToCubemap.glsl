@type vertex

layout (location = 0) in vec3 aPosition;

out vec3 vWorldPosition;

uniform mat4 uProjection;
uniform mat4 uView;

void main() {
  vWorldPosition = aPosition;
  gl_Position =  uProjection * uView * vec4(vWorldPosition, 1.0);
}

@type fragment

precision mediump float;
precision highp int;

out vec4 vFragmentColor;

in vec3 vWorldPosition;
uniform sampler2D uEquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v) {
  vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
  uv *= invAtan;
  uv += 0.5;
  return uv;
}

// Tone-mapping
vec3 aces(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
  vec2 uv = SampleSphericalMap(normalize(vWorldPosition));
  vec3 color = texture(uEquirectangularMap, uv).rgb;

  color = aces(color);

  vFragmentColor = vec4(color, 1.0);
}
