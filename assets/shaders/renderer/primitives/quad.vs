#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in uint aTexIndex;

out vec4 vColor;
out vec2 vTexCoord;
flat out uint  vTexIndex;

uniform mat4 uProjectionView = mat4(1.0f);

void main() {
   vColor      = aColor;
   vTexCoord   = aTexCoord;
   vTexIndex   = aTexIndex;
   gl_Position = uProjectionView * vec4(aPosition, 1.0);
}
