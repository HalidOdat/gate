struct Material {
  uint  diffuse;
  uint  specular;
  uint  emission;
  uint  padding0;
  uint  padding1;
  uint  padding2;
  float shininess;
  float transparency;
};

struct Light {
  vec3  position;
  vec3  direction;
  float cutOff;
  float outerCutOff;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float linear;
  float quadratic;
};

@type vertex

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexture;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in mat4 aModelMatrix;  // 3, 4, 5, 6
layout (location = 7) in mat3 aNormalMatrix; // 7, 8, 9

layout (std140) uniform Camera {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  mat4 projectionViewMatrix;
  vec4 position;
  vec4 front;
} uCamera;

layout (std140) uniform Materials {
  Material uMaterials[];
};

uniform Light uLight;

out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vFragmentPosition;
flat out vec3 vViewPosition;
flat out Material vMaterial;
flat out Light vLight;

void main() {
  vTexCoords        = aTexture;
  vNormal           = aNormalMatrix * aNormal;
  vFragmentPosition = vec3(aModelMatrix * vec4(aPosition, 1.0));
  vViewPosition     = vec3(uCamera.position);
  vMaterial         = uMaterials[0];
  vLight            = uLight;

  gl_Position       = uCamera.projectionViewMatrix * vec4(vFragmentPosition, 1.0);
}

@type fragment

precision mediump float;

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragmentPosition;
flat in vec3 vViewPosition;
flat in Material vMaterial;
flat in Light vLight;

uniform sampler2D uTextures[16];

out vec4 vFragmentColor;

void main() {
  vec3 diffuseColor = vec3(texture(uTextures[vMaterial.diffuse], vTexCoords));

  // ambient
  vec3 ambient = vLight.ambient * diffuseColor;
  
  // diffuse 
  vec3 lightDir = normalize(vLight.position - vFragmentPosition);
  vec3 norm = normalize(vNormal);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = vLight.diffuse * diff * diffuseColor;

  // specular
  vec3 viewDir    = normalize(vViewPosition - vFragmentPosition);
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(viewDir, halfwayDir), 0.0), vMaterial.shininess * 4.0f);
  vec3 specular = vLight.specular * spec * vec3(texture(uTextures[vMaterial.specular], vTexCoords));

  vec3 emission = vec3(texture(uTextures[vMaterial.emission], vTexCoords));
    
  // // check if lighting is inside the spotlight cone
  // // soft edges
  // float theta = dot(lightDir, normalize(-vLight.direction)); 
  // float epsilon   = vLight.cutOff - vLight.outerCutOff;
  // float intensity = clamp((theta - vLight.outerCutOff) / epsilon, 0.0, 1.0);
  // diffuse  *= intensity;
  // specular *= intensity;
  // emission *= intensity;

  // light attenuation
  float distance    = length(vLight.position - vFragmentPosition);
  float attenuation = 1.0 / (1.0f + vLight.linear * distance + vLight.quadratic * (distance * distance));
  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;
  // emission *= attenuation;

  vec3 result = ambient + diffuse + specular + emission;
  vFragmentColor = vec4(result, vMaterial.transparency);
}