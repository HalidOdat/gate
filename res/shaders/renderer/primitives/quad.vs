#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

uniform mat4 uTransform;
uniform mat4 uProjectionView;

void main() {
   vTexCoord    = aTexCoord;
   gl_Position = uProjectionView * uTransform * vec4(aPosition, 1.0);
}
