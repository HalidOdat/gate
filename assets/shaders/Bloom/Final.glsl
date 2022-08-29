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

out vec4 vFragmentColor;

void main() {
  vFragmentColor = texture(uTexture, vTexCoords);
}
