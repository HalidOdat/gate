#version 330 core

@type vertex

layout (location = 0) in vec3  aPosition;
layout (location = 1) in float aRadius;
layout (location = 2) in vec4  aColor;

out vec3 vPosition;
out vec4 vColor;

uniform mat4 uProjectionView;

void main() {
  vColor      = aColor;
  vPosition   = aPosition;
  gl_Position = uProjectionView * vec4(aPosition, 1.0);
}

@type fragment

in vec3 vPosition;
in vec4 vColor;

out vec4 FragColor;

float circle(vec2 position, float radius, float feather) {
  return smoothstep(radius, radius + feather, dot(position, position) * 4.0);
}

void main() {
  FragColor = Vec4(vec3( circle(UV, 0.2, 0.005) ), 1.0f); // * vColor;
}
