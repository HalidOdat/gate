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

precision lowp float;

in vec2 vPosition;
in vec2 vTexCoords;

uniform sampler2D uTexture;
uniform float uFilterRadius;

out vec4 vFragmentColor;

void main() {
  const float x = 0.005f;
  const float y = 0.005f;

  // Take 9 samples around current texel:
  // a - b - c
  // d - e - f
  // g - h - i
  // === ('e' is the current texel) ===
  vec3 a = texture(uTexture, vec2(vTexCoords.x - x, vTexCoords.y + y)).rgb;
  vec3 b = texture(uTexture, vec2(vTexCoords.x,     vTexCoords.y + y)).rgb;
  vec3 c = texture(uTexture, vec2(vTexCoords.x + x, vTexCoords.y + y)).rgb;

  vec3 d = texture(uTexture, vec2(vTexCoords.x - x, vTexCoords.y)).rgb;
  vec3 e = texture(uTexture, vec2(vTexCoords.x,     vTexCoords.y)).rgb;
  vec3 f = texture(uTexture, vec2(vTexCoords.x + x, vTexCoords.y)).rgb;

  vec3 g = texture(uTexture, vec2(vTexCoords.x - x, vTexCoords.y - y)).rgb;
  vec3 h = texture(uTexture, vec2(vTexCoords.x,     vTexCoords.y - y)).rgb;
  vec3 i = texture(uTexture, vec2(vTexCoords.x + x, vTexCoords.y - y)).rgb;

  // Apply weighted distribution, by using a 3x3 tent filter:
  //  1   | 1 2 1 |
  // -- * | 2 4 2 |
  // 16   | 1 2 1 |
  vec3 upsample = e*4.0;
  upsample += (b+d+f+h)*2.0;
  upsample += (a+c+g+i);
  upsample *= 1.0 / 16.0;

  vFragmentColor = vec4(upsample, 1.0f);

  // vFragmentColor = texture(uTexture, vTexCoords);
}
