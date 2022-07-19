#pragma once

#include "Renderer/Camera.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Texture.hpp"

namespace Game {
  
  class Renderer {
  public:
    Renderer();

    void begin(const Camera& camera);
    void draw(const Ref<Shader>& shader, const Ref<VertexArray>& vao, const Ref<Texture2D>& texture);
    void end();

  private:
    Mat4 projectionViewMatrix;
  };

} // namespace Game
