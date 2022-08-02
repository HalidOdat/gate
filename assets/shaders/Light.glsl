#version 330 core

@type vertex

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexture;
layout (location = 2) in vec3 aNormal;

out vec2 vTexture;
out vec3 vNormal;
out vec3 vFragmentPosition;

uniform mat4 uProjectionViewMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;

void main() {
  vTexture = aTexture;
  vNormal  = normalize(uNormalMatrix * aNormal);
  vFragmentPosition = vec3(uModelMatrix * vec4(aPosition, 1.0));
  gl_Position = uProjectionViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
}

@type fragment

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

out vec4 vFragmentColor;

in vec2 vTexture;
in vec3 vNormal;
in vec3 vFragmentPosition;

uniform sampler2D uTexture;
uniform vec3      uViewPosition;

uniform Material uMaterial;
uniform Light    uLight;

void main() {
  vec3 ambient = uLight.ambient * uMaterial.ambient;

  vec3 lightDirection = normalize(uLight.position - vFragmentPosition);

  float diff = max(dot(vNormal, lightDirection), 0.0);
  vec3 diffuse = uLight.diffuse * (diff * uMaterial.diffuse);

  vec3 viewDirection = normalize(uViewPosition - vFragmentPosition);
  vec3 reflectDirirection = reflect(-lightDirection, vNormal); 

  float spec = pow(max(dot(viewDirection, reflectDirirection), 0.0), uMaterial.shininess);
  vec3 specular = uLight.specular * (spec * uMaterial.specular);

  vFragmentColor = texture(uTexture, vTexture) * vec4(ambient + diffuse + specular, 1.0f);
}