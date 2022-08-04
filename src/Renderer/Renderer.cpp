#include <array>
#include <cctype>
#include <algorithm>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/Renderer.hpp"
#include "Resource/Manager.hpp"
#include "Application.hpp"

namespace Game {

  // TODO: Use uniform buffer for camera and static data

  // TODO: Refactor this
  struct QuadBatch {
    QuadBatch(
      Ref<VertexArray>  vertexArray,
      Ref<VertexBuffer> vertexBuffer,
      Shader::Handle       shader,
      Texture2D::Handle    whiteTexture
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

    Shader::Handle       shader;
    Ref<VertexBuffer> vertexBuffer;
    Ref<VertexArray>  vertexArray;

    // TODO: Query OpenGL
    static constexpr const u32 MAX_TEXTURES = 32;

    std::vector<Texture2D::Handle> textures;

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
    Texture2D::Handle texture;
    std::array<Vec4, 96> coords;
  };

  struct RenderUnit {
    using InitFn = auto(*)(Shader& shader) -> void;

    Shader::Handle shader;
    Mesh::Handle   mesh;
    Material    material;

    Mat4 modelMatrix;
    Mat3 normalMatrix;

    InitFn init;
  };

  struct RenderCamera {
    Mat4 projection;
    Mat4 view;
    
    Vec3 position;
    Vec3 front;
  };

  struct RenderPipeline {
    Shader::Handle postProcesingShader;
    RenderCamera camera;
    Ref<FrameBuffer> frameBuffer;
    Ref<VertexArray> quadVertexArray;

    std::vector<RenderUnit>          units;
    std::vector<u32>                 opaqueUnitIndices;
    std::vector<std::pair<f32, u32>> transparentUnitIndices; // distance from camera and index
  };

  struct RendererData {
    Mat4 projectionViewMatrix;
    Mat4 projectionMatrix;
    Mat4 ViewMatrix;

    // TODO: Make accessor for this texture in ResourceManager
    Texture2D::Handle whiteTexture;
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
    
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);

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
    shader->bind();
    shader->setIntArray("uTextures", samples, QuadBatch::MAX_TEXTURES);

    auto quad = QuadBatch(vertexArray, vertexBuffer, shader, whiteTexture);

    Texture2D::Specification specification;
    specification.filtering.mag = Texture::FilteringMode::Nearest;
    specification.filtering.min = Texture::FilteringMode::Linear;
    specification.mipmap        = Texture::MipmapMode::Linear;

    auto fontTexture = ResourceManager::loadTexture("PixelFont_7x9_112x54.png", specification);
    const auto fontTextureWidth  = fontTexture->getWidth();
    const auto fontTextureHeight = fontTexture->getHeight();
    const auto fontCharacterWidth  = 7;
    const auto fontCharacterHeight = 9;

    GAME_DEBUG_ASSERT(fontTextureWidth % fontCharacterWidth == 0);
    GAME_DEBUG_ASSERT(fontTextureHeight % fontCharacterHeight == 0);

    auto postProcesingShader = ResourceManager::loadShader("PostProcessing.glsl");
    renderer = new RendererData{
      Mat4(1.0f),
      Mat4(1.0f),
      Mat4(1.0f),
      whiteTexture,
      {vertexArray, vertexBuffer, shader, whiteTexture},
      {fontTexture},
      {postProcesingShader}
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

    auto width = Application::getWindow().getWidth();
    auto height = Application::getWindow().getHeight();
    renderer->pipeline.frameBuffer = FrameBuffer::create(width, height);

    static const float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
      // positions   // texCoords
      -1.0f,  1.0f,  0.0f, 1.0f,
      -1.0f, -1.0f,  0.0f, 0.0f,
       1.0f, -1.0f,  1.0f, 0.0f,

      -1.0f,  1.0f,  0.0f, 1.0f,
       1.0f, -1.0f,  1.0f, 0.0f,
       1.0f,  1.0f,  1.0f, 1.0f
    };

    auto quadVertexArray = VertexArray::create();
    auto quadVertexBuffer = VertexBuffer::create(quadVertices);
    quadVertexBuffer->setLayout({
      { BufferElement::Type::Float2, /* position */ },
      { BufferElement::Type::Float2  /* texture */ },
    });
    quadVertexArray->addVertexBuffer(quadVertexBuffer);
    quadVertexArray->unbind();
    renderer->pipeline.quadVertexArray = quadVertexArray;
  }

  void Renderer::shutdown() {
    delete renderer;
    renderer = nullptr;
  }

  void Renderer::invalidate(u32 width, u32 height) {
    renderer->pipeline.frameBuffer->invalidate(width, height);
  }

  void Renderer::begin(const Camera& camera) {
    renderer->projectionViewMatrix = camera.getProjectionViewMatrix();
    renderer->projectionMatrix     = camera.getProjectionMatrix();
    renderer->ViewMatrix           = camera.getViewMatrix();
  }
  
  void Renderer::begin3D(const PerspectiveCameraController& cameraController) {
    renderer->pipeline.camera.projection = cameraController.getCamera().getProjectionMatrix();
    renderer->pipeline.camera.view       = cameraController.getCamera().getViewMatrix();
    renderer->pipeline.camera.position   = cameraController.getPosition();
    renderer->pipeline.camera.front      = cameraController.getFront();
  }

  void Renderer::submit(Shader::Handle& shader, const Mesh::Handle& mesh, const Material& material, const Mat4& transform) {
    // Don't render fully transparent objects
    if (material.getTransparency() == 0.0f) {
      return;
    }

    u32 index = (u32)renderer->pipeline.units.size();
    RenderUnit unit {
      shader,
      mesh,
      material,
      transform,
      Mat3(glm::transpose(glm::inverse(transform))),
    };
    renderer->pipeline.units.push_back(unit);
    if (!material.hasTransparency()) {
      renderer->pipeline.opaqueUnitIndices.emplace_back(index);
    } else {
      Vec3 position = {transform[3][0], transform[3][1], transform[3][2]};
      f32 distance = glm::length(position - renderer->pipeline.camera.position);
      renderer->pipeline.transparentUnitIndices.push_back(std::pair(distance, index));
    }
  }

  void renderUnit(u32 unitIndex) {
    RenderUnit& unit = renderer->pipeline.units[unitIndex];

    unit.shader->bind();
    unit.shader->setMat4("uProjectionMatrix", renderer->pipeline.camera.projection);
    unit.shader->setMat4("uViewMatrix", renderer->pipeline.camera.view);

    unit.shader->setMat4("uModelMatrix", unit.modelMatrix);
    unit.shader->setMat3("uNormalMatrix", unit.normalMatrix);

    // TODO: Make this more dynamic
    unit.material.getDiffuseMap()->bind(0);
    unit.material.getSpecularMap()->bind(1);
    unit.material.getEmissionMap()->bind(2);

    unit.shader->setInt("uMaterial.diffuse",  0);
    unit.shader->setInt("uMaterial.specular", 1);
    unit.shader->setInt("uMaterial.emission", 2);

    unit.shader->setFloat("uMaterial.shininess", unit.material.getShininess());
    unit.shader->setFloat("uMaterial.transparency", unit.material.getTransparency());

    auto vao = unit.mesh->getVertexArray();
    vao->bind();
    vao->drawIndices();
    vao->unbind();
  }

  void Renderer::waitAndRender() {
    // First Pass
    renderer->pipeline.frameBuffer->bind();

    Renderer::enableDepthTest(true);

    // Renderer::enableCullFace(true);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CCW);

    GAME_GL_CHECK(glClearColor(0.2f, 0.2f, 0.2f, 1.0f));
    GAME_GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    
    for (auto& index : renderer->pipeline.opaqueUnitIndices) {
      renderUnit(index);
    }

    std::sort(
      renderer->pipeline.transparentUnitIndices.begin(),
      renderer->pipeline.transparentUnitIndices.end(),
      [](auto& a, auto& b) {
        return a.first > b.first;
      }
    );

    for (auto& unitIndex : renderer->pipeline.transparentUnitIndices) {
      renderUnit(unitIndex.second);
    }

    renderer->pipeline.units.clear();
    renderer->pipeline.opaqueUnitIndices.clear();
    renderer->pipeline.transparentUnitIndices.clear();

    // Renderer::enableCullFace(false);

    Renderer::enableDepthTest(false);
    renderer->pipeline.frameBuffer->unbind();
    
    //  glDisable(GL_DEPTH_TEST);

    // Second Pass
    GAME_GL_CHECK(glClearColor(1.0f, 1.0f, 1.0f, 1.0f)); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    GAME_GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    u32 texture = renderer->pipeline.frameBuffer->getColorAttachmentId();
    GAME_GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GAME_GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture));
    renderer->pipeline.postProcesingShader->bind();
    renderer->pipeline.postProcesingShader->setInt("uScreenTexture", 0);

    renderer->pipeline.quadVertexArray->bind();
    renderer->pipeline.quadVertexArray->drawArrays(6);
    renderer->pipeline.quadVertexArray->unbind();

    // Renderer::enableDepthTest(false);
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
    *(renderer->quad.current++) = { renderer->projectionViewMatrix * transform * QuadBatch::position[0], color, /* {1.0f, 1.0f} */ {tc.z, tc.w}, index }; // top-right
    *(renderer->quad.current++) = { renderer->projectionViewMatrix * transform * QuadBatch::position[1], color, /* {1.0f, 0.0f} */ {tc.z, tc.y}, index }; // bottom-right
    *(renderer->quad.current++) = { renderer->projectionViewMatrix * transform * QuadBatch::position[2], color, /* {0.0f, 0.0f} */ {tc.x, tc.y}, index }; // bottom-left
    *(renderer->quad.current++) = { renderer->projectionViewMatrix * transform * QuadBatch::position[3], color, /* {0.0f, 1.0f} */ {tc.x, tc.w}, index }; // top-left

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

  void Renderer::drawQuad(const Vec3& position, const Vec2& size, const Texture2D::Handle& texture, const Vec4& color) {
    Mat4 transform = Mat4(1.0f);
    transform      = glm::translate(transform, position);
    transform      = glm::scale(transform, Vec3(size, 1.0f));
    Renderer::drawQuad(transform, texture, color);
  }

  void Renderer::drawQuad(const Mat4& transform, const Texture2D::Handle& texture, const Vec4& color) {
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

    *(renderer->quad.current++) = { renderer->projectionViewMatrix * transform * QuadBatch::position[0], color, {1.0f, 1.0f}, index }; // top-right
    *(renderer->quad.current++) = { renderer->projectionViewMatrix * transform * QuadBatch::position[1], color, {1.0f, 0.0f}, index }; // bottom-right
    *(renderer->quad.current++) = { renderer->projectionViewMatrix * transform * QuadBatch::position[2], color, {0.0f, 0.0f}, index }; // bottom-left
    *(renderer->quad.current++) = { renderer->projectionViewMatrix * transform * QuadBatch::position[3], color, {0.0f, 1.0f}, index }; // top-left

    renderer->quad.count++;
  }

  void Renderer::flush() {
    if (renderer->quad.count) {
      renderer->whiteTexture->bind();

      for (u32 i = 0; i < renderer->quad.textures.size(); ++i) {
        renderer->quad.textures[i]->bind(i);
      }

      renderer->quad.shader->bind();

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
