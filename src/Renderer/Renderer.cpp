#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Renderer/Renderer.hpp"
#include "Resource/Manager.hpp"

namespace Game {

  // TODO: Refactor this
  struct QuadBatch {
    QuadBatch(
      Ref<VertexArray>  vertexArray,
      Ref<VertexBuffer> vertexBuffer,
      Shader            shader,
      Texture2D         whiteTexture
    )
    : vertexArray{vertexArray},
      vertexBuffer{vertexBuffer},
      shader{shader}
    {
      this->base    = new QuadBatch::Vertex[QuadBatch::MAX * QuadBatch::VERTICES_COUNT];
      this->current = this->base;
      this->count = 0;

      this->textures.reserve(QuadBatch::MAX_TEXTURES);
      this->textures.push_back(whiteTexture);
    }

    DISALLOW_MOVE_AND_COPY(QuadBatch);

    ~QuadBatch() {
      delete[] this->base;
    }

    static constexpr const u32 MAX              = 512;
    static constexpr const u32 VERTICES_COUNT   = 4;
    static constexpr const u32 INDICES_COUNT    = 6;
    
    static constexpr const auto VERTEX_SHADER   = "renderer/primitives/quad.vs";
    static constexpr const auto FRAGMENT_SHADER = "renderer/primitives/quad.fs";

    struct Vertex {
      Vec3 position;
      Vec4 color;
      Vec2 texCoord;
      u32  texIndex;
    };

    Shader            shader;
    Ref<VertexBuffer> vertexBuffer;
    Ref<VertexArray>  vertexArray;

    // TODO: Query OpenGL
    static constexpr const u32 MAX_TEXTURES = 32;

    std::vector<Texture2D> textures;

    Vertex* base    = nullptr;
    Vertex* current = nullptr;
    usize count     = 0;

    static constexpr const u32 VERTEX_BUFFER_BYTE_SIZE = MAX * VERTICES_COUNT * sizeof(Vertex);
    static constexpr const u32 INDEX_BUFFER_COUNT      = MAX * INDICES_COUNT;
  };

  struct RendererData {
    Mat4 projectionViewMatrix;
    Texture2D whiteTexture;

    QuadBatch quad;
  };

  static RendererData* renderer;

  void Renderer::Initialize() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    static const u8 bytes[] = { 0xFF, 0xFF, 0xFF };
    auto whiteTexture = ResourceManager::textureFromBytes(bytes, 1, 1, 3);

    auto vertexArray = VertexArray::create();
    auto vertexBuffer = VertexBuffer::withSize(QuadBatch::VERTEX_BUFFER_BYTE_SIZE);
    vertexBuffer->setLayout({
      { BufferElement::Type::Float3, /* position */ },
      { BufferElement::Type::Float4  /* color */ },
      { BufferElement::Type::Float2  /* texture */ },
      { BufferElement::Type::Uint    /* texIndex */ },
    });
    vertexArray->addVertexBuffer(vertexBuffer);

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
    vertexArray->setIndexBuffer(indexBuffer);
    vertexArray->unbind();

    auto shader = ResourceManager::loadShader(QuadBatch::VERTEX_SHADER, QuadBatch::FRAGMENT_SHADER);

    i32 samples[QuadBatch::MAX_TEXTURES];
    for (u32 i = 0; i < QuadBatch::MAX_TEXTURES; ++i) {
      samples[i] = i;
    }
    shader.bind();
    shader.setIntArray("uTextures", samples, QuadBatch::MAX_TEXTURES);

    auto quad = QuadBatch(vertexArray, vertexBuffer, shader, whiteTexture);

    renderer = new RendererData{
      Mat4(1.0f),
      whiteTexture,
      {vertexArray, vertexBuffer, shader, whiteTexture}
    };
  }

  void Renderer::Shutdown() {
    delete renderer;
    renderer = nullptr;
  }

  void Renderer::begin(const Camera& camera) {
    renderer->projectionViewMatrix = camera.getProjectionViewMatrix();
  }

  void Renderer::draw(const Ref<Shader>& shader, const Ref<VertexArray>& vao, const Ref<Texture2D>& texture) {
    shader->bind();
    shader->setMat4("uProjectionView", renderer->projectionViewMatrix);

    texture->bind();
    vao->bind();
    vao->drawIndices();
  }

  void Renderer::drawQuad(const Vec2& position, const Vec2& size, const Vec4& color) {
    Renderer::drawQuad(Vec3(position, 0.0f), size, color);
  }

  void Renderer::drawQuad(const Vec3& position, const Vec2& size, const Vec4& color) {
    Renderer::drawQuad(position, size, renderer->whiteTexture, color);
  }

  void Renderer::drawQuad(const Vec3& position, const Vec2& size, const Texture2D& texture, const Vec4& color) {
    if (renderer->quad.count == QuadBatch::MAX) {
      Renderer::flush();
    }

    u32 index = 0;
    for (; index < renderer->quad.textures.size(); ++index) {
      if (renderer->quad.textures[index] == texture) {
        break;
      }
    }

    if (index == renderer->quad.textures.size()) {
      if (renderer->quad.textures.size() >= QuadBatch::MAX_TEXTURES) {
        Renderer::flush();
      }

      index = (u32)renderer->quad.textures.size();
      renderer->quad.textures.push_back(texture);
    }

    // TODO: do transfrom with projection view matrix here
    *(renderer->quad.current++) = { position + Vec3{size.x,    0.0f, 0.0f}, color, {1.0f, 1.0f}, index }; // top-right
    *(renderer->quad.current++) = { position + Vec3{size.x, -size.y, 0.0f}, color, {1.0f, 0.0f}, index }; // bottom-right
    *(renderer->quad.current++) = { position + Vec3{0.0f,   -size.y, 0.0f}, color, {0.0f, 0.0f}, index }; // bottom-left
    *(renderer->quad.current++) = { position + Vec3{0.0f,      0.0f, 0.0f}, color, {0.0f, 1.0f}, index }; // top-left

    renderer->quad.count++;
  }

  void Renderer::flush() {
    if (renderer->quad.count) {
      usize dataLength = (u8*)renderer->quad.current - (u8*)renderer->quad.base;
      renderer->whiteTexture.bind();

      for (u32 i = 0; i < renderer->quad.textures.size(); ++i) {
        renderer->quad.textures[i].bind(i);
      }

      renderer->quad.shader.bind();
      renderer->quad.shader.setMat4("uProjectionView", renderer->projectionViewMatrix);

      renderer->quad.vertexArray->bind();
      renderer->quad.vertexBuffer->set({renderer->quad.base, dataLength});
      renderer->quad.vertexArray->drawIndices(renderer->quad.count * QuadBatch::INDICES_COUNT);

      renderer->quad.current = renderer->quad.base;
      renderer->quad.count = 0;

      renderer->quad.textures.clear();
      renderer->quad.textures.push_back(renderer->whiteTexture);
    }
  }

  void Renderer::end() {
    Renderer::flush();
    // nothing... (for now)
  }

} // namespace Game
