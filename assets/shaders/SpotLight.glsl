#version 330 core

@type vertex

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexture;
layout (location = 2) in vec3 aNormal;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

uniform mat3 uNormalMatrix;

out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vFragmentPosition;

void main() {
  vTexCoords        = aTexture;
  vNormal           = uNormalMatrix * aNormal;
  vFragmentPosition = vec3(uModelMatrix * vec4(aPosition, 1.0));

  gl_Position       = uProjectionMatrix * uViewMatrix * vec4(vFragmentPosition, 1.0);
}

@type fragment

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float     shininess;
  float     transparency;
};

struct Light {
  vec3  position;
  vec3  direction;
  float cutOff;
  float outerCutOff;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

in vec2          vTexCoords;
in vec3          vNormal;
in vec3          vFragmentPosition;

uniform Material uMaterial;
uniform Light    uLight;
uniform vec3     uViewPosition;

out vec4 vFragmentColor;

void main() {
  // ambient
  vec3 ambient = uLight.ambient * vec3(texture(uMaterial.diffuse, vTexCoords));
  
  // diffuse 
  vec3 lightDir = normalize(uLight.position - vFragmentPosition);
  vec3 norm = normalize(vNormal);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = uLight.diffuse * diff * vec3(texture(uMaterial.diffuse, vTexCoords));
    
  // specular
  vec3 viewDir    = normalize(uViewPosition - vFragmentPosition);
  vec3 halfwayDir = normalize(lightDir + viewDir);
  // vec3 reflectDir = reflect(-lightDir, norm);  
  float spec = pow(max(dot(viewDir, halfwayDir), 0.0), uMaterial.shininess * 4.0f);
  vec3 specular = uLight.specular * spec * vec3(texture(uMaterial.specular, vTexCoords));

  // emission
  // vec3 emission = vec3(texture(uMaterial.emission, vTexCoords));
    
  // check if lighting is inside the spotlight cone
  // soft edges
  float theta = dot(lightDir, normalize(-uLight.direction)); 
  float epsilon   = uLight.cutOff - uLight.outerCutOff;
  float intensity = clamp((theta - uLight.outerCutOff) / epsilon, 0.0, 1.0);  
  diffuse  *= intensity;
  specular *= intensity;
  //emission *= intensity;

  // light attenuation
  float distance    = length(uLight.position - vFragmentPosition);
  float attenuation = 1.0 / (uLight.constant + uLight.linear * distance + uLight.quadratic * (distance * distance));
  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;
  //emission *= attenuation;

  vec3 result = ambient + diffuse + specular; // + emission
  vFragmentColor = vec4(result, uMaterial.transparency);
}