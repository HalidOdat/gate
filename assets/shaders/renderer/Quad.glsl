@type vertex

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in uint aTexIndex;

out vec4 vColor;
out vec2 vTexCoord;
flat out uint vTexIndex;

void main() {
   vColor      = aColor;
   vTexCoord   = aTexCoord;
   vTexIndex   = aTexIndex;
   gl_Position = vec4(aPosition, 1.0);
}

@type fragment

precision mediump float;

out vec4 FragColor;

in vec4 vColor;
in vec2 vTexCoord;
flat in uint vTexIndex;

uniform sampler2D uTextures[16];

void main() {
   // Reason for the switch cases: 
   // https://stackoverflow.com/questions/72648980/opengl-sampler2d-array
   vec4 color;
   switch (vTexIndex) {
      case  0u: color = texture(uTextures[ 0], vTexCoord); break;
      case  1u: color = texture(uTextures[ 1], vTexCoord); break;
      case  2u: color = texture(uTextures[ 2], vTexCoord); break;
      case  3u: color = texture(uTextures[ 3], vTexCoord); break;
      case  4u: color = texture(uTextures[ 4], vTexCoord); break;
      case  5u: color = texture(uTextures[ 5], vTexCoord); break;
      case  6u: color = texture(uTextures[ 6], vTexCoord); break;
      case  7u: color = texture(uTextures[ 7], vTexCoord); break;
      case  8u: color = texture(uTextures[ 8], vTexCoord); break;
      case  9u: color = texture(uTextures[ 9], vTexCoord); break;
      case 10u: color = texture(uTextures[10], vTexCoord); break;
      case 11u: color = texture(uTextures[11], vTexCoord); break;
      case 12u: color = texture(uTextures[12], vTexCoord); break;
      case 13u: color = texture(uTextures[13], vTexCoord); break;
      case 14u: color = texture(uTextures[14], vTexCoord); break;
      case 15u: color = texture(uTextures[15], vTexCoord); break;
   }
   FragColor = color * vColor;
}
