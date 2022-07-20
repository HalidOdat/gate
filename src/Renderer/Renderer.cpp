#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Renderer/Renderer.hpp"

namespace Game {

  struct QuadBatch {
    static constexpr const u32 MAX            = 512;
    static constexpr const u32 VERTICES_COUNT = 4;
    static constexpr const u32 INDICES_COUNT  = 6;
    
    static constexpr const auto VERTEX_SHADER   = "res/shaders/renderer/primitives/quad.vs";
    static constexpr const auto FRAGMENT_SHADER = "res/shaders/renderer/primitives/quad.fs";

    struct Vertex {
      Vec3 position;
      Vec4 color;
      Vec2 texCoord;
    };

    Ref<Shader>       shader;
    Ref<VertexBuffer> vertexBuffer;
    Ref<VertexArray>  vertexArray;

    Vertex* base    = nullptr;
    Vertex* current = nullptr;
    usize count     = 0;

    static constexpr const u32 VERTEX_BUFFER_BYTE_SIZE = MAX * VERTICES_COUNT * sizeof(Vertex);
    static constexpr const u32 INDEX_BUFFER_COUNT      = MAX * INDICES_COUNT;
  };

  struct RendererData {
    Mat4 projectionViewMatrix;
    Ref<Texture2D> whiteTexture;

    QuadBatch quad;
  };

  static RendererData renderer;

  void Renderer::Initialize() {
    static const u8 bytes[] = { 0xFF, 0xFF, 0xFF };
    renderer.whiteTexture = Texture2D::fromBytes(bytes, 1, 1, 3);

    renderer.quad.vertexArray = VertexArray::create();
    renderer.quad.vertexBuffer = VertexBuffer::withSize(QuadBatch::VERTEX_BUFFER_BYTE_SIZE);
    renderer.quad.vertexBuffer->setLayout({
      { BufferElement::Type::Float3, /* position */ },
      { BufferElement::Type::Float4  /* color */ },
      { BufferElement::Type::Float2  /* texture */ },
    });
    renderer.quad.vertexArray->addVertexBuffer(renderer.quad.vertexBuffer);

    u32* indices = new u32[QuadBatch::INDEX_BUFFER_COUNT];
    u32  offset = 0;
    for (u32 i = 0; i < QuadBatch::INDEX_BUFFER_COUNT; i += QuadBatch::INDICES_COUNT) {
      indices[0 + i] = 0 + offset;
      indices[1 + i] = 1 + offset;
      indices[2 + i] = 2 + offset;

      indices[3 + i] = 2 + offset;
      indices[4 + i] = 3 + offset;
      indices[5 + i] = 0 + offset;

      offset += QuadBatch::VERTICES_COUNT;
    }

    auto indexBuffer = IndexBuffer::create({indices, QuadBatch::INDEX_BUFFER_COUNT});
    delete[] indices;
    renderer.quad.vertexArray->setIndexBuffer(indexBuffer);
    renderer.quad.vertexArray->unbind();

    renderer.quad.shader = Shader::create(QuadBatch::VERTEX_SHADER, QuadBatch::FRAGMENT_SHADER);

    renderer.quad.base    = new QuadBatch::Vertex[QuadBatch::MAX * QuadBatch::VERTICES_COUNT];
    renderer.quad.current = renderer.quad.base;
    renderer.quad.count = 0;
  }

  void Renderer::Shutdown() {
    renderer.whiteTexture.reset();

    renderer.quad.shader.reset();
    renderer.quad.vertexBuffer.reset();
    renderer.quad.vertexArray.reset();

    delete[] renderer.quad.base;
  }

  void Renderer::begin(const Camera& camera) {
    renderer.projectionViewMatrix = camera.getProjectionViewMatrix();
  }

  void Renderer::draw(const Ref<Shader>& shader, const Ref<VertexArray>& vao, const Ref<Texture2D>& texture) {
    shader->bind();
    shader->setMat4("uProjectionView", renderer.projectionViewMatrix);

    texture->bind();
    vao->bind();
    vao->drawIndices(6);
  }

  void Renderer::drawQuad(const Vec2& position, const Vec2& size, const Vec4& color) {
    Renderer::drawQuad(Vec3(position, 0.0f), size, color);
  }

  void Renderer::drawQuad(const Vec3& position, const Vec2& size, const Vec4& color) {
    if (renderer.quad.count == QuadBatch::MAX) {
      Renderer::flush();
    }

    // TODO: do transfrom with projection view matrix here
    *(renderer.quad.current++) = { position + Vec3{size.x,    0.0f, 0.0f}, color, {1.0f, 1.0f} }; // top-right
    *(renderer.quad.current++) = { position + Vec3{size.x, -size.y, 0.0f}, color, {1.0f, 0.0f} }; // bottom-right
    *(renderer.quad.current++) = { position + Vec3{0.0f,   -size.y, 0.0f}, color, {0.0f, 0.0f} }; // bottom-left
    *(renderer.quad.current++) = { position + Vec3{0.0f,      0.0f, 0.0f}, color, {0.0f, 1.0f} }; // top-left

    renderer.quad.count++;
  }

  void Renderer::flush() {
    if (renderer.quad.count) {
      usize dataLength = (u8*)renderer.quad.current - (u8*)renderer.quad.base;
      renderer.whiteTexture->bind();

      renderer.quad.shader->bind();
      renderer.quad.shader->setMat4("uProjectionView", renderer.projectionViewMatrix);
      renderer.quad.shader->setInt("uTexture", 0);

      renderer.quad.vertexArray->bind();
      renderer.quad.vertexBuffer->set({renderer.quad.base, dataLength});
      renderer.quad.vertexArray->drawIndices(renderer.quad.count * QuadBatch::INDICES_COUNT);

      renderer.quad.current = renderer.quad.base;
      renderer.quad.count = 0;
    }
  }

  void Renderer::end() {
    Renderer::flush();
    // nothing... (for now)
  }

} // namespace Game
