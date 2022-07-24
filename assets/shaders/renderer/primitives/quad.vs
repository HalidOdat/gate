#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 vColor;
out vec2 vTexCoord;

uniform mat4 uProjectionView = mat4(1.0f);

void main() {
   vColor      = aColor;
   vTexCoord   = aTexCoord;
   gl_Position = uProjectionView * vec4(aPosition, 1.0);
}
