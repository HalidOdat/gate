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

uniform sampler2D uScreenTexture;
uniform vec2 uResolution;

out vec4 vFragmentColor;

void main() {
  #if 0
    ivec2 texturePosition = ivec2(gl_FragCoord.x, gl_FragCoord.y);
    vec4 colorSample0 = texelFetch(tex, texturePosition, 0);

    vec4 colorSample1 = texelFetch(tex, texturePosition, 1);
    vec4 colorSample2 = texelFetch(tex, texturePosition, 2);
    vec4 colorSample3 = texelFetch(tex, texturePosition, 3);

    vec4 antialiased = (colorSample0 + colorSample1 + colorSample2 + colorSample3) / 4.0f;
    vFragmentColor = antialiased;
  #else
    vec2 texelSize = 1.0 / uResolution;
    float x = texelSize.x;
    float y = texelSize.y;

    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ('e' is the current texel) ===
    vec3 a = texture(uScreenTexture, vec2(vTexCoords.x - 2.0f * x, vTexCoords.y + 2.0f*y)).rgb;
    vec3 b = texture(uScreenTexture, vec2(vTexCoords.x,       vTexCoords.y + 2.0f * y)).rgb;
    vec3 c = texture(uScreenTexture, vec2(vTexCoords.x + 2.0f * x, vTexCoords.y + 2.0f*y)).rgb;

    vec3 d = texture(uScreenTexture, vec2(vTexCoords.x - 2.0f*x, vTexCoords.y)).rgb;
    vec3 e = texture(uScreenTexture, vec2(vTexCoords.x,       vTexCoords.y)).rgb;
    vec3 f = texture(uScreenTexture, vec2(vTexCoords.x + 2.0f*x, vTexCoords.y)).rgb;

    vec3 g = texture(uScreenTexture, vec2(vTexCoords.x - 2.0f*x, vTexCoords.y - 2.0f*y)).rgb;
    vec3 h = texture(uScreenTexture, vec2(vTexCoords.x,       vTexCoords.y - 2.0f*y)).rgb;
    vec3 i = texture(uScreenTexture, vec2(vTexCoords.x + 2.0f*x, vTexCoords.y - 2.0f*y)).rgb;

    vec3 j = texture(uScreenTexture, vec2(vTexCoords.x - x, vTexCoords.y + y)).rgb;
    vec3 k = texture(uScreenTexture, vec2(vTexCoords.x + x, vTexCoords.y + y)).rgb;
    vec3 l = texture(uScreenTexture, vec2(vTexCoords.x - x, vTexCoords.y - y)).rgb;
    vec3 m = texture(uScreenTexture, vec2(vTexCoords.x + x, vTexCoords.y - y)).rgb;

    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap,
    // so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    // contribute 0.5 to the final color output. The code below is written
    // to effectively yield this sum. We get:
    // 0.125*5 + 0.03125*4 + 0.0625*4 = 1

    vec3 downsample = e*0.125;
         downsample += (a+c+g+i)*0.03125;
         downsample += (b+d+f+h)*0.0625;
         downsample += (j+k+l+m)*0.125;

    // TODO: Apply Karis average

    vFragmentColor = vec4(downsample, 1.0f);
  #endif
}