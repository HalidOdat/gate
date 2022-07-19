#include "Renderer/Renderer.hpp"

namespace Game {
  
  Renderer::Renderer() {}

  void Renderer::begin(const Camera& camera) {
    this->projectionViewMatrix = camera.getProjectionViewMatrix();
  }

  void Renderer::draw(const Ref<Shader>& shader, const Ref<VertexArray>& vao, const Ref<Texture2D>& texture) {
    shader->bind();
    shader->setMat4("uProjectionView", this->projectionViewMatrix);

    texture->bind();
    vao->bind();

    vao->draw();
  }

  void Renderer::end() {
    // nothing... (for now)
  }


} // namespace Game
