precision highp float;
precision highp int;

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

layout (location =  0) in vec3 aPosition;
layout (location =  1) in vec2 aTexture;
layout (location =  2) in vec3 aNormal;
layout (location =  3) in mat4 aModelMatrix;  // 3, 4, 5, 6
layout (location =  7) in mat3 aNormalMatrix; // 7, 8, 9
layout (location = 10) in highp uvec4 aEntityId;

layout (std140) uniform Camera {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  mat4 projectionViewMatrix;
  vec4 position;
  vec4 front;
  vec4 clippingPlane;
} uCamera;

layout (std140) uniform Materials {
  Material uMaterials[MAX_MATERIALS];
};

uniform Light uLight;

out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vFragmentPosition;
flat out vec3 vViewPosition;
flat out Material vMaterial;
flat out Light vLight;

flat out highp uvec4 vEntityId;

void main() {
  vTexCoords        = aTexture;
  vNormal           = aNormalMatrix * aNormal;
  vFragmentPosition = vec3(aModelMatrix * vec4(aPosition, 1.0));
  vViewPosition     = vec3(uCamera.position);
  vMaterial         = uMaterials[0];
  vLight            = uLight;
  vEntityId         = aEntityId;

  gl_Position       = uCamera.projectionViewMatrix * vec4(vFragmentPosition, 1.0);
}

@type fragment

// #if !GL_ES
//   layout(early_fragment_tests) in;
// #endif

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragmentPosition;
flat in vec3 vViewPosition;
flat in Material vMaterial;
flat in Light vLight;

flat in highp uvec4 vEntityId;

uniform sampler2D uTextures[16];

layout (location = 0) out vec4 vFragmentColor;
layout (location = 1) out highp uvec4 vEntityIdOut;

void main() {
  // vec4 diffuseTexture;
  //  switch (vMaterial.diffuse) {
  //   case  0u: diffuseTexture = texture(uTextures[ 0], vTexCoords); break;
  //   case  1u: diffuseTexture = texture(uTextures[ 1], vTexCoords); break;
  //   case  2u: diffuseTexture = texture(uTextures[ 2], vTexCoords); break;
  //   case  3u: diffuseTexture = texture(uTextures[ 3], vTexCoords); break;
  //   case  4u: diffuseTexture = texture(uTextures[ 4], vTexCoords); break;
  //   case  5u: diffuseTexture = texture(uTextures[ 5], vTexCoords); break;
  //   case  6u: diffuseTexture = texture(uTextures[ 6], vTexCoords); break;
  //   case  7u: diffuseTexture = texture(uTextures[ 7], vTexCoords); break;
  //   case  8u: diffuseTexture = texture(uTextures[ 8], vTexCoords); break;
  //   case  9u: diffuseTexture = texture(uTextures[ 9], vTexCoords); break;
  //   case 10u: diffuseTexture = texture(uTextures[10], vTexCoords); break;
  //   case 11u: diffuseTexture = texture(uTextures[11], vTexCoords); break;
  //   case 12u: diffuseTexture = texture(uTextures[12], vTexCoords); break;
  //   case 13u: diffuseTexture = texture(uTextures[13], vTexCoords); break;
  //   case 14u: diffuseTexture = texture(uTextures[14], vTexCoords); break;
  //   case 15u: diffuseTexture = texture(uTextures[15], vTexCoords); break;
  // }
  // vec3 diffuseColor = vec3(diffuseTexture);

  // // ambient
  // vec3 ambient = vLight.ambient * diffuseColor;
  
  // // diffuse 
  // vec3 lightDir = normalize(vLight.position - vFragmentPosition);
  // vec3 norm = normalize(vNormal);
  // float diff = max(dot(norm, lightDir), 0.0);
  // vec3 diffuse = vLight.diffuse * diff * diffuseColor;

  // // specular
  // vec3 viewDir    = normalize(vViewPosition - vFragmentPosition);
  // vec3 halfwayDir = normalize(lightDir + viewDir);
  // float spec = pow(max(dot(viewDir, halfwayDir), 0.0), vMaterial.shininess * 4.0f);
  // vec4 specularTexture;
  // switch (vMaterial.specular) {
  //   case  0u: specularTexture = texture(uTextures[ 0], vTexCoords); break;
  //   case  1u: specularTexture = texture(uTextures[ 1], vTexCoords); break;
  //   case  2u: specularTexture = texture(uTextures[ 2], vTexCoords); break;
  //   case  3u: specularTexture = texture(uTextures[ 3], vTexCoords); break;
  //   case  4u: specularTexture = texture(uTextures[ 4], vTexCoords); break;
  //   case  5u: specularTexture = texture(uTextures[ 5], vTexCoords); break;
  //   case  6u: specularTexture = texture(uTextures[ 6], vTexCoords); break;
  //   case  7u: specularTexture = texture(uTextures[ 7], vTexCoords); break;
  //   case  8u: specularTexture = texture(uTextures[ 8], vTexCoords); break;
  //   case  9u: specularTexture = texture(uTextures[ 9], vTexCoords); break;
  //   case 10u: specularTexture = texture(uTextures[10], vTexCoords); break;
  //   case 11u: specularTexture = texture(uTextures[11], vTexCoords); break;
  //   case 12u: specularTexture = texture(uTextures[12], vTexCoords); break;
  //   case 13u: specularTexture = texture(uTextures[13], vTexCoords); break;
  //   case 14u: specularTexture = texture(uTextures[14], vTexCoords); break;
  //   case 15u: specularTexture = texture(uTextures[15], vTexCoords); break;
  // }
  // vec3 specular = vLight.specular * spec * vec3(specularTexture);

  // vec4 emissionTexture;
  // switch (vMaterial.emission) {
  //   case  0u: emissionTexture = texture(uTextures[ 0], vTexCoords); break;
  //   case  1u: emissionTexture = texture(uTextures[ 1], vTexCoords); break;
  //   case  2u: emissionTexture = texture(uTextures[ 2], vTexCoords); break;
  //   case  3u: emissionTexture = texture(uTextures[ 3], vTexCoords); break;
  //   case  4u: emissionTexture = texture(uTextures[ 4], vTexCoords); break;
  //   case  5u: emissionTexture = texture(uTextures[ 5], vTexCoords); break;
  //   case  6u: emissionTexture = texture(uTextures[ 6], vTexCoords); break;
  //   case  7u: emissionTexture = texture(uTextures[ 7], vTexCoords); break;
  //   case  8u: emissionTexture = texture(uTextures[ 8], vTexCoords); break;
  //   case  9u: emissionTexture = texture(uTextures[ 9], vTexCoords); break;
  //   case 10u: emissionTexture = texture(uTextures[10], vTexCoords); break;
  //   case 11u: emissionTexture = texture(uTextures[11], vTexCoords); break;
  //   case 12u: emissionTexture = texture(uTextures[12], vTexCoords); break;
  //   case 13u: emissionTexture = texture(uTextures[13], vTexCoords); break;
  //   case 14u: emissionTexture = texture(uTextures[14], vTexCoords); break;
  //   case 15u: emissionTexture = texture(uTextures[15], vTexCoords); break;
  // }
  // vec3 emission = vec3(emissionTexture);
    
  // // // check if lighting is inside the spotlight cone
  // // // soft edges
  // // float theta = dot(lightDir, normalize(-vLight.direction)); 
  // // float epsilon   = vLight.cutOff - vLight.outerCutOff;
  // // float intensity = clamp((theta - vLight.outerCutOff) / epsilon, 0.0, 1.0);
  // // diffuse  *= intensity;
  // // specular *= intensity;
  // // emission *= intensity;

  // // light attenuation
  // float distance    = length(vLight.position - vFragmentPosition);
  // float attenuation = 1.0 / (1.0f + vLight.linear * distance + vLight.quadratic * (distance * distance));
  // ambient  *= attenuation;
  // diffuse  *= attenuation;
  // specular *= attenuation;
  // emission *= attenuation;

  // vec3 result = ambient + diffuse + specular + emission;
  // vFragmentColor = vec4(result, vMaterial.transparency);

  // lightingShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
  // lightingShader.setVec3("viewPos", camera.Position);
  //
  // // light properties
  // lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
  // lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
  // lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
  vec3 light_direction = vec3(-0.2f, -1.0f, -0.3f);
  vec3 light_ambient   = vec3(0.3f, 0.3f, 0.3f);
  vec3 light_diffuse   = vec3(0.5f, 0.5f, 0.5f);
  vec3 light_specular  = vec3(1.0f, 1.0f, 1.0f);

  // ambient
  vec4 diffuseTexture;
  switch (vMaterial.diffuse) {
    case  0u: diffuseTexture = texture(uTextures[ 0], vTexCoords); break;
    case  1u: diffuseTexture = texture(uTextures[ 1], vTexCoords); break;
    case  2u: diffuseTexture = texture(uTextures[ 2], vTexCoords); break;
    case  3u: diffuseTexture = texture(uTextures[ 3], vTexCoords); break;
    case  4u: diffuseTexture = texture(uTextures[ 4], vTexCoords); break;
    case  5u: diffuseTexture = texture(uTextures[ 5], vTexCoords); break;
    case  6u: diffuseTexture = texture(uTextures[ 6], vTexCoords); break;
    case  7u: diffuseTexture = texture(uTextures[ 7], vTexCoords); break;
    case  8u: diffuseTexture = texture(uTextures[ 8], vTexCoords); break;
    case  9u: diffuseTexture = texture(uTextures[ 9], vTexCoords); break;
    case 10u: diffuseTexture = texture(uTextures[10], vTexCoords); break;
    case 11u: diffuseTexture = texture(uTextures[11], vTexCoords); break;
    case 12u: diffuseTexture = texture(uTextures[12], vTexCoords); break;
    case 13u: diffuseTexture = texture(uTextures[13], vTexCoords); break;
    case 14u: diffuseTexture = texture(uTextures[14], vTexCoords); break;
    case 15u: diffuseTexture = texture(uTextures[15], vTexCoords); break;
  }
  vec3 ambient = light_ambient * diffuseTexture.rgb;
  
  // diffuse
  vec4 specularTexture;
  switch (vMaterial.specular) {
    case  0u: specularTexture = texture(uTextures[ 0], vTexCoords); break;
    case  1u: specularTexture = texture(uTextures[ 1], vTexCoords); break;
    case  2u: specularTexture = texture(uTextures[ 2], vTexCoords); break;
    case  3u: specularTexture = texture(uTextures[ 3], vTexCoords); break;
    case  4u: specularTexture = texture(uTextures[ 4], vTexCoords); break;
    case  5u: specularTexture = texture(uTextures[ 5], vTexCoords); break;
    case  6u: specularTexture = texture(uTextures[ 6], vTexCoords); break;
    case  7u: specularTexture = texture(uTextures[ 7], vTexCoords); break;
    case  8u: specularTexture = texture(uTextures[ 8], vTexCoords); break;
    case  9u: specularTexture = texture(uTextures[ 9], vTexCoords); break;
    case 10u: specularTexture = texture(uTextures[10], vTexCoords); break;
    case 11u: specularTexture = texture(uTextures[11], vTexCoords); break;
    case 12u: specularTexture = texture(uTextures[12], vTexCoords); break;
    case 13u: specularTexture = texture(uTextures[13], vTexCoords); break;
    case 14u: specularTexture = texture(uTextures[14], vTexCoords); break;
    case 15u: specularTexture = texture(uTextures[15], vTexCoords); break;
  }
  vec3 norm = normalize(vNormal);

  // vec3 lightDir = normalize(light.position - FragPos);
  vec3 lightDir = normalize(-light_direction);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light_diffuse * diff * diffuseTexture.rgb;
  
  // specular
  vec3 viewDir = normalize(vFragmentPosition - vFragmentPosition);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), vMaterial.shininess);
  vec3 specular = light_specular * spec * specularTexture.rgb;

  vec3 result = ambient + diffuse + specular;
  vFragmentColor = vec4(result, 1.0);
  vEntityIdOut = vEntityId;
  // vEntityIdOut = vec4(vEntityId.rgb, 1.0f);
}
