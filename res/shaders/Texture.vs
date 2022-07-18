#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 Color;
out vec2 TexCoord;

uniform mat4 uTransform      = mat4(1.0f);
uniform mat4 uProjectionView = mat4(1.0f);

uniform vec2 uAtlasSize   = vec2(1.0f, 1.0f);
uniform vec2 uAtlasOffset = vec2(0.0f, 0.0f);

void main() {
   Color       = aColor;
   TexCoord    = (aTexCoord + uAtlasOffset) / uAtlasSize;
   gl_Position = uProjectionView * uTransform * vec4(aPosition, 1.0);
}