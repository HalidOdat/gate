#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/Renderer.hpp"

namespace Game {
  static const float SQUARE_VERICES[] = {
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, // top left 
  };

  static const u32 SQUARE_INDICES[] = {
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
  };

  static constexpr const auto SQUARE_VERTEX_SHADER   = "res/shaders/renderer/primitives/quad.vs";
  static constexpr const auto SQUARE_FRAGMENT_SHADER = "res/shaders/renderer/primitives/quad.fs";

  struct RendererData {
    Mat4 projectionViewMatrix;

    Ref<Shader> squareShader;
    Ref<VertexArray> squareVertexArray;

    Ref<Texture2D> whiteTexture;
  };

  static RendererData renderer;

  void Renderer::Initialize() {
    renderer.squareVertexArray = VertexArray::create();
    auto vertexBuffer = VertexBuffer::create(SQUARE_VERICES);
    vertexBuffer->setLayout({
      { BufferElement::Type::Float3 },
      { BufferElement::Type::Float2 },
    });

    renderer.squareVertexArray->addVertexBuffer(vertexBuffer);
    auto indexBuffer = IndexBuffer::create(SQUARE_INDICES);
    renderer.squareVertexArray->setIndexBuffer(indexBuffer);
    renderer.squareVertexArray->unbind();
    
    renderer.squareShader = Shader::create(SQUARE_VERTEX_SHADER, SQUARE_FRAGMENT_SHADER);

    const u8 bytes[] = { 0xFF, 0xFF, 0xFF };
    renderer.whiteTexture = Texture2D::fromBytes(bytes, 1, 1, 3);
  }

  void Renderer::Shutdown() {
    renderer.squareShader.reset();
    renderer.squareVertexArray.reset();
    renderer.whiteTexture.reset();
  }

  void Renderer::begin(const Camera& camera) {
    renderer.projectionViewMatrix = camera.getProjectionViewMatrix();
  }

  void Renderer::draw(const Ref<Shader>& shader, const Ref<VertexArray>& vao, const Ref<Texture2D>& texture) {
    shader->bind();
    shader->setMat4("uProjectionView", renderer.projectionViewMatrix);

    texture->bind();
    vao->bind();
    vao->draw();
  }

  void Renderer::drawQuad(const Vec2& position, const Vec2& size, const Vec4& color) {
    auto transform = Mat4(1.0f);
    transform = glm::translate(transform, Vec3(position, 0.0f));
    transform = glm::scale(transform, Vec3(size, 1.0f));

    renderer.whiteTexture->bind();
    renderer.squareShader->bind();
    renderer.squareShader->setMat4("uProjectionView", renderer.projectionViewMatrix);
    renderer.squareShader->setMat4("uTransform", transform);
    renderer.squareShader->setVec4("uColor", color);
    renderer.squareShader->setInt("uTexture", 0);
    renderer.squareVertexArray->bind();
    renderer.squareVertexArray->draw();
  }

  void Renderer::end() {
    // nothing... (for now)
  }


} // namespace Game
