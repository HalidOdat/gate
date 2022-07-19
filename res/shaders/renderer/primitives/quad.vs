#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 vColor;
out vec2 vTexCoord;

uniform mat4 uTransform;
uniform mat4 uProjectionView;

void main() {
   vColor       = aColor;
   vTexCoord    = aTexCoord;
   gl_Position = uProjectionView * uTransform * vec4(aPosition, 1.0);
}
