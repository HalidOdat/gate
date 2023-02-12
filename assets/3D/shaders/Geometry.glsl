precision highp float;
precision highp int;

struct Material {
  uint  diffuse;
  uint  specular;
  uint  padding0;
  uint  padding1;
  uint  padding2;
  uint  padding3;
  float shininess;
  float transparency;
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

out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vFragmentPosition;
flat out vec3 vViewPosition;
flat out Material vMaterial;

flat out highp uvec4 vEntityId;

void main() {
  vTexCoords        = aTexture;
  vNormal           = aNormalMatrix * aNormal;
  vFragmentPosition = vec3(aModelMatrix * vec4(aPosition, 1.0));
  vViewPosition     = vec3(uCamera.position);
  vMaterial         = uMaterials[0];
  vEntityId         = aEntityId;

  gl_Position       = uCamera.projectionViewMatrix * vec4(vFragmentPosition, 1.0);
}

@type fragment

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragmentPosition;
flat in vec3 vViewPosition;
flat in Material vMaterial;
flat in highp uvec4 vEntityId;

uniform sampler2D uTextures[16];

layout (location = 0) out vec4 vFragmentColor;
layout (location = 1) out highp uvec4 vEntityIdOut;

vec3 getTextureColor(uint textureIndex) {
  vec4 color;
  switch (textureIndex) {
    case  0u: color = texture(uTextures[ 0], vTexCoords); break;
    case  1u: color = texture(uTextures[ 1], vTexCoords); break;
    case  2u: color = texture(uTextures[ 2], vTexCoords); break;
    case  3u: color = texture(uTextures[ 3], vTexCoords); break;
    case  4u: color = texture(uTextures[ 4], vTexCoords); break;
    case  5u: color = texture(uTextures[ 5], vTexCoords); break;
    case  6u: color = texture(uTextures[ 6], vTexCoords); break;
    case  7u: color = texture(uTextures[ 7], vTexCoords); break;
    case  8u: color = texture(uTextures[ 8], vTexCoords); break;
    case  9u: color = texture(uTextures[ 9], vTexCoords); break;
    case 10u: color = texture(uTextures[10], vTexCoords); break;
    case 11u: color = texture(uTextures[11], vTexCoords); break;
    case 12u: color = texture(uTextures[12], vTexCoords); break;
    case 13u: color = texture(uTextures[13], vTexCoords); break;
    case 14u: color = texture(uTextures[14], vTexCoords); break;
    case 15u: color = texture(uTextures[15], vTexCoords); break;
  }
  return vec3(color);
}

void main() {
  vec3 light_direction = vec3(-0.2f, -1.0f, -0.3f);
  vec3 light_ambient   = vec3(0.5f, 0.5f, 0.5f);
  vec3 light_diffuse   = vec3(0.5f, 0.5f, 0.5f);
  vec3 light_specular  = vec3(1.0f, 1.0f, 1.0f);

  vec3 diffuseTexture = getTextureColor(vMaterial.diffuse);
  vec3 specularTexture = getTextureColor(vMaterial.specular);

  vec3 norm = normalize(vNormal);
  vec3 viewDir = normalize(vViewPosition - vFragmentPosition);
  vec3 lightDir = normalize(-light_direction);
  vec3 halfwayDir = normalize(lightDir + viewDir);

  // diffuse shading
  float diff = max(dot(vNormal, lightDir), 0.0f);

  // specular shading
  // vec3 reflectDir = reflect(-lightDir, vNormal);
  float spec = pow(max(dot(viewDir, halfwayDir), 0.0f), vMaterial.shininess);

  // combine results
  vec3 ambient  = light_ambient  * diffuseTexture;
  vec3 diffuse  = light_diffuse  * diff * diffuseTexture;
  vec3 specular = light_specular * spec * specularTexture;

  vFragmentColor = vec4(ambient + diffuse + specular, 1.0f);
  vEntityIdOut = vEntityId;
}
