#version 330 core

@type vertex

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexture;
layout (location = 2) in vec3 aNormal;

out vec2 vTexture;
out vec3 vNormal;
out vec3 vFragmentPosition;

uniform mat4 uProjectionView;
uniform mat4 uTransform;

void main() {
  vTexture = aTexture;
  vNormal  = aNormal;
  vFragmentPosition = vec3(uTransform * vec4(aPosition, 1.0));
  gl_Position = uProjectionView * uTransform * vec4(aPosition, 1.0);
}

@type fragment

out vec4 vFragmentColor;

in vec2 vTexture;
in vec3 vNormal;
in vec3 vFragmentPosition;

uniform sampler2D uTexture;
uniform vec3      uLightPosition = vec3(4.0f, 0.0f, 0.0f);

void main() {
  const float ambientStrength = 0.1;
  const vec3  lightColor = vec3(1.0f, 1.0f, 1.0f);
  const vec3  ambient = ambientStrength * lightColor;

  vec3 norm = normalize(vNormal);
  vec3 lightDirection = normalize(uLightPosition - vFragmentPosition);

  float diff = max(dot(norm, lightDirection), 0.0);
  vec3 diffuse = diff * lightColor;

  vFragmentColor = texture(uTexture, vTexture) * vec4(ambient + diffuse, 1.0f);
}