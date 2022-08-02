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
  float     shininess;
};

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
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
  vec3 ambient = uLight.ambient * texture(uMaterial.diffuse, vTexCoords).rgb;
  	
  // diffuse 
  vec3 norm = normalize(vNormal);
  vec3 lightDir = normalize(uLight.position - vFragmentPosition);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = uLight.diffuse * diff * vec3(texture(uMaterial.diffuse, vTexCoords));
    
  // specular
  vec3 viewDir = normalize(uViewPosition - vFragmentPosition);
  vec3 reflectDir = reflect(-lightDir, norm);  
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
  vec3 specular = uLight.specular * spec * vec3(texture(uMaterial.specular, vTexCoords));
        
  vec3 result = ambient + diffuse + specular;
  vFragmentColor = vec4(result, 1.0);
}