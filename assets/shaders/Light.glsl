#version 330 core

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

@type vertex

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexture;
layout (location = 2) in vec3 aNormal;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;

uniform Material uMaterial;
uniform Light    uLight;

out vec2 vTexture;
out vec3 vNormal;
out vec3 vFragmentPosition;
out vec3 vLightPosition;

flat out Material vMaterial;
flat out Light    vLight;

void main() {
  vTexture          = aTexture;
  vNormal           = uNormalMatrix * aNormal;
  vFragmentPosition = vec3(uViewMatrix * uModelMatrix * vec4(aPosition, 1.0));
  vLightPosition    = vec3(uViewMatrix * vec4(uLight.position, 1.0f));
  gl_Position       = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);

  vMaterial         = uMaterial;
  vLight            = uLight;
}

@type fragment

in vec2     vTexture;
in vec3     vNormal;
in vec3     vFragmentPosition;
in vec3     vLightPosition;
flat in Material vMaterial;
flat in Light    vLight;

uniform sampler2D uTexture;
uniform vec3      uViewPosition;

out vec4 vFragmentColor;

void main() {
  vec3 ambient = vLight.ambient * vMaterial.ambient;

  vec3 norm = normalize(vNormal);
  vec3 lightDirection = normalize(vLightPosition - vFragmentPosition);

  float diff = max(dot(norm, lightDirection), 0.0);
  vec3 diffuse = vLight.diffuse * (diff * vMaterial.diffuse);

  vec3 viewDirection = normalize(-vFragmentPosition);
  vec3 reflectDirirection = reflect(-lightDirection, norm); 

  float spec = pow(max(dot(viewDirection, reflectDirirection), 0.0), vMaterial.shininess);
  vec3 specular = vLight.specular * (spec * vMaterial.specular);

  vFragmentColor = texture(uTexture, vTexture) * vec4(ambient + diffuse + specular, 1.0f);
}