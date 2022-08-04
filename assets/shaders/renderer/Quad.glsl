#version 330 core

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

out vec4 FragColor;

in vec4 vColor;
in vec2 vTexCoord;
flat in uint vTexIndex;

uniform sampler2D uTextures[32];

void main() {
   // Reason for the switch cases: 
   // https://stackoverflow.com/questions/72648980/opengl-sampler2d-array
   vec4 color;
   switch (vTexIndex) {
      case  0: color = texture(uTextures[ 0], vTexCoord); break;
      case  1: color = texture(uTextures[ 1], vTexCoord); break;
      case  2: color = texture(uTextures[ 2], vTexCoord); break;
      case  3: color = texture(uTextures[ 3], vTexCoord); break;
      case  4: color = texture(uTextures[ 4], vTexCoord); break;
      case  5: color = texture(uTextures[ 5], vTexCoord); break;
      case  6: color = texture(uTextures[ 6], vTexCoord); break;
      case  7: color = texture(uTextures[ 7], vTexCoord); break;
      case  8: color = texture(uTextures[ 8], vTexCoord); break;
      case  9: color = texture(uTextures[ 9], vTexCoord); break;
      case 10: color = texture(uTextures[10], vTexCoord); break;
      case 11: color = texture(uTextures[11], vTexCoord); break;
      case 12: color = texture(uTextures[12], vTexCoord); break;
      case 13: color = texture(uTextures[13], vTexCoord); break;
      case 14: color = texture(uTextures[14], vTexCoord); break;
      case 15: color = texture(uTextures[15], vTexCoord); break;
      case 16: color = texture(uTextures[16], vTexCoord); break;
      case 17: color = texture(uTextures[17], vTexCoord); break;
      case 18: color = texture(uTextures[18], vTexCoord); break;
      case 19: color = texture(uTextures[19], vTexCoord); break;
      case 20: color = texture(uTextures[20], vTexCoord); break;
      case 21: color = texture(uTextures[21], vTexCoord); break;
      case 22: color = texture(uTextures[22], vTexCoord); break;
      case 23: color = texture(uTextures[23], vTexCoord); break;
      case 24: color = texture(uTextures[24], vTexCoord); break;
      case 25: color = texture(uTextures[25], vTexCoord); break;
      case 26: color = texture(uTextures[26], vTexCoord); break;
      case 27: color = texture(uTextures[27], vTexCoord); break;
      case 28: color = texture(uTextures[28], vTexCoord); break;
      case 29: color = texture(uTextures[29], vTexCoord); break;
      case 30: color = texture(uTextures[30], vTexCoord); break;
      case 31: color = texture(uTextures[31], vTexCoord); break;
   }
   FragColor = color * vColor;
}
