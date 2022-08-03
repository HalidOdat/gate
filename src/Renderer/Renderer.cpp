#include <array>
#include <cctype>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Renderer/Renderer.hpp"
#include "Resource/Manager.hpp"

namespace Game {

  // TODO: Use uniform buffer for camera and static data

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
    
    static constexpr const auto SHADER_PATH = "renderer/Quad.glsl";

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
    u32     count   = 0;

    static constexpr const u32 VERTEX_BUFFER_BYTE_SIZE = MAX * VERTICES_COUNT * sizeof(Vertex);
    static constexpr const u32 INDEX_BUFFER_COUNT      = MAX * INDICES_COUNT;

    static constexpr const std::array<Vec4, 4> position = {
      Vec4{ 1.0f,  1.0f, 0.0f, 1.0f }, // top    right
		  Vec4{ 1.0f,  0.0f, 0.0f, 1.0f }, // bottom right
		  Vec4{ 0.0f,  0.0f, 0.0f, 1.0f }, // bottom left
		  Vec4{ 0.0f,  1.0f, 0.0f, 1.0f }, // top    left 
    };
  };

  struct FontData {
    Texture2D texture;
    std::array<Vec4, 96> coords;
  };

  struct RenderUnit {
    using InitFn = auto(*)(Shader& shader) -> void;

    Shader shader;
    Mesh   mesh;
    Material material;

    Mat4 modelMatrix;
    Mat3 normalMatrix;

    InitFn init;
  };

  struct RenderPipeline {
    Mat4 projectionMatrix;
    Mat4 viewMatrix;

    std::vector<RenderUnit> units;
    std::vector<u32>        opaqueUnitIndices;
  };

  struct RendererData {
    Mat4 projectionViewMatrix;
    Mat4 projectionMatrix;
    Mat4 ViewMatrix;

    // TODO: Make accessor for this texture in ResourceManager
    Texture2D whiteTexture;
    QuadBatch quad;

    FontData font;

    // 3D stuff
    RenderPipeline pipeline;
  };

  static RendererData* renderer;

  void Renderer::enableBlending(bool yes) {
    if (yes) {
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_BLEND);
    }
  }

  void Renderer::enableDepthTest(bool yes) {
    if (yes) {
      glEnable(GL_DEPTH_TEST);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }

  void Renderer::enableCullFace(bool yes) {
    if (yes) {
      glEnable(GL_CULL_FACE);
    } else {
      glDisable(GL_CULL_FACE);
    }
  }

  void Renderer::initialize() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

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

    auto shader = ResourceManager::loadShader(QuadBatch::SHADER_PATH);

    i32 samples[QuadBatch::MAX_TEXTURES];
    for (u32 i = 0; i < QuadBatch::MAX_TEXTURES; ++i) {
      samples[i] = i;
    }
    shader.bind();
    shader.setIntArray("uTextures", samples, QuadBatch::MAX_TEXTURES);

    auto quad = QuadBatch(vertexArray, vertexBuffer, shader, whiteTexture);

    auto fontTexture = ResourceManager::loadTexture("PixelFont_7x9_112x54.png", false);
    const auto fontTextureWidth  = fontTexture.getWidth();
    const auto fontTextureHeight = fontTexture.getHeight();
    const auto fontCharacterWidth  = 7;
    const auto fontCharacterHeight = 9;

    GAME_DEBUG_ASSERT(fontTextureWidth % fontCharacterWidth == 0);
    GAME_DEBUG_ASSERT(fontTextureHeight % fontCharacterHeight == 0);

    renderer = new RendererData{
      Mat4(1.0f),
      Mat4(1.0f),
      Mat4(1.0f),
      whiteTexture,
      {vertexArray, vertexBuffer, shader, whiteTexture},
      {fontTexture}
    };

    u32 count = 0;
    for (u32 height = 0; height != fontTextureHeight; height += fontCharacterHeight) {
      for (u32 width = 0; width != fontTextureWidth; width += fontCharacterWidth) {
        const f32 x     = (f32)width  / fontTextureWidth;
        const f32 y     = (f32)height / fontTextureHeight;
        const f32 xSize = (f32)fontCharacterWidth / fontTextureWidth;
        const f32 ySize = (f32)fontCharacterHeight / fontTextureHeight;
        renderer->font.coords[count++] = Vec4{
          Vec2{ x,         1 - y - ySize },
          Vec2{ x + xSize, 1 - y - ySize / fontCharacterHeight },
        };
      }
    }
  }

  void Renderer::shutdown() {
    delete renderer;
    renderer = nullptr;
  }

  void Renderer::begin(const Camera& camera) {
    renderer->projectionViewMatrix = camera.getProjectionViewMatrix();
    renderer->projectionMatrix     = camera.getProjectionMatrix();
    renderer->ViewMatrix           = camera.getViewMatrix();
  }

  void Renderer::begin3D(const Camera& camera) {
    renderer->pipeline.projectionMatrix     = camera.getProjectionMatrix();
    renderer->pipeline.viewMatrix           = camera.getViewMatrix();
  }

  void Renderer::submit(Shader& shader, const Mesh& mesh, const Material& material, const Mat4& transform) {
    u32 index = (u32)renderer->pipeline.units.size();
    RenderUnit unit {
      shader,
      mesh,
      material,
      transform,
      Mat3(glm::transpose(glm::inverse(transform))),
    };
    renderer->pipeline.units.push_back(unit);
    renderer->pipeline.opaqueUnitIndices.emplace_back(index);
  }

  void Renderer::waitAndRender() {
    for (auto& index : renderer->pipeline.opaqueUnitIndices) {
      RenderUnit& unit = renderer->pipeline.units[index];

      unit.shader.bind();
      unit.shader.setMat4("uProjectionMatrix", renderer->pipeline.projectionMatrix);
      unit.shader.setMat4("uViewMatrix", renderer->pipeline.viewMatrix);

      unit.shader.setMat4("uModelMatrix", unit.modelMatrix);
      unit.shader.setMat3("uNormalMatrix", unit.normalMatrix);

      // TODO: Make this more dynamic
      unit.material.getDiffuseMap().bind(0);
      unit.material.getSpecularMap().bind(1);
      unit.material.getEmissionMap().bind(2);

      unit.shader.setInt("uMaterial.diffuse",  0);
      unit.shader.setInt("uMaterial.specular", 1);
      unit.shader.setInt("uMaterial.emission", 2);

      unit.shader.setFloat("uMaterial.shininess", unit.material.getShininess());

      auto vao = unit.mesh.getVertexArray();
      vao->bind();
      vao->drawIndices();
      vao->unbind();
    }

    renderer->pipeline.units.clear();
    renderer->pipeline.opaqueUnitIndices.clear();
  }

  void Renderer::drawQuad(const Vec2& position, const Vec2& size, const Vec4& color) {
    Renderer::drawQuad(Vec3(position, 0.0f), size, color);
  }

  void Renderer::drawQuad(const Vec3& position, const Vec2& size, const Vec4& color) {
    Renderer::drawQuad(position, size, renderer->whiteTexture, color);
  }

  void Renderer::drawChar(char c, const Vec3& position, const Vec2& size, const Vec4& color) {
    Mat4 transform = Mat4(1.0f);
    transform      = glm::translate(transform, position);
    transform      = glm::scale(transform, Vec3(size, 1.0f));
    Renderer::drawChar(c, transform, color);
  }

  void Renderer::drawChar(char c, const Mat4& transform, const Vec4& color) {
    auto texture = renderer->font.texture;

    // TODO: refactor this.
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

    if (!std::isprint(c)) {
      c = (usize)('~' + 1);
    }

    Vec4 tc = renderer->font.coords[(usize)(c - ' ')];

    // TODO: do transfrom with projection view matrix here
    *(renderer->quad.current++) = { transform * QuadBatch::position[0], color, /* {1.0f, 1.0f} */ {tc.z, tc.w}, index }; // top-right
    *(renderer->quad.current++) = { transform * QuadBatch::position[1], color, /* {1.0f, 0.0f} */ {tc.z, tc.y}, index }; // bottom-right
    *(renderer->quad.current++) = { transform * QuadBatch::position[2], color, /* {0.0f, 0.0f} */ {tc.x, tc.y}, index }; // bottom-left
    *(renderer->quad.current++) = { transform * QuadBatch::position[3], color, /* {0.0f, 1.0f} */ {tc.x, tc.w}, index }; // top-left

    renderer->quad.count++;
  }

  void Renderer::drawText(const StringView& text, const Vec2& position, const float size, const Vec4& color) {
    Renderer::drawText(text, Vec3(position, 0.1f), {size - size/8.0f, size}, color);
  }

  void Renderer::drawText(const StringView& text, const Vec3& position, const Vec2& size, const Vec4& color) {
    Vec3 start = position;
    Vec3 current = start;
    for (const char c : text) {
      if (c != '\n') {
        Renderer::drawChar(c, current, size, color);
        current.x += size.x;
      } else {
        current.y -= size.y; 
        current.x  = start.x;
      }
    }
  }

  void Renderer::drawQuad(const Vec3& position, const Vec2& size, const Texture2D& texture, const Vec4& color) {
    Mat4 transform = Mat4(1.0f);
    transform      = glm::translate(transform, position);
    transform      = glm::scale(transform, Vec3(size, 1.0f));
    Renderer::drawQuad(transform, texture, color);
  }

  void Renderer::drawQuad(const Mat4& transform, const Texture2D& texture, const Vec4& color) {
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
    *(renderer->quad.current++) = { transform * QuadBatch::position[0], color, {1.0f, 1.0f}, index }; // top-right
    *(renderer->quad.current++) = { transform * QuadBatch::position[1], color, {1.0f, 0.0f}, index }; // bottom-right
    *(renderer->quad.current++) = { transform * QuadBatch::position[2], color, {0.0f, 0.0f}, index }; // bottom-left
    *(renderer->quad.current++) = { transform * QuadBatch::position[3], color, {0.0f, 1.0f}, index }; // top-left

    renderer->quad.count++;
  }

  void Renderer::flush() {
    if (renderer->quad.count) {
      renderer->whiteTexture.bind();

      for (u32 i = 0; i < renderer->quad.textures.size(); ++i) {
        renderer->quad.textures[i].bind(i);
      }

      renderer->quad.shader.bind();
      renderer->quad.shader.setMat4("uProjectionView", renderer->projectionViewMatrix);

      renderer->quad.vertexArray->bind();
      renderer->quad.vertexBuffer->set({renderer->quad.base,  renderer->quad.count * QuadBatch::VERTICES_COUNT});
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
