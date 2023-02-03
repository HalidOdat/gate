@type vertex

layout (location = 0) in vec2  aWorldPosition;
layout (location = 1) in vec2  aLocalPosition;
layout (location = 2) in vec4  aColor;
layout (location = 3) in float aThickness;
layout (location = 4) in float aFade;

out vec2  vLocalPosition;
flat out vec4  vColor;
flat out float vThickness;
flat out float vFade;

void main() {
    vLocalPosition = aLocalPosition;
    vColor         = aColor;
    vThickness     = aThickness;
    vFade          = aFade;

    gl_Position = vec4(aWorldPosition, 0.0f, 1.0f);
}

@type fragment

precision mediump float;

in vec2 vLocalPosition;
flat in vec4 vColor;
flat in float vThickness;
flat in float vFade;

layout (location = 0) out vec4 FragColor;

void main() {
  float distance = 1.0 - length(vLocalPosition);
  float circle = smoothstep(0.0, vFade, distance);
  circle *= smoothstep(vThickness + vFade, vThickness, distance);

  if (circle == 0.0) {
    discard;
  }

  FragColor = vColor;
  FragColor.a *= circle;
}
