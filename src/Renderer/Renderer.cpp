#include <array>
#include <cctype>

#include "Core/OpenGL.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Base.hpp"
#include "Application.hpp"

#include "Renderer/Renderer.hpp"

namespace Gate {

  Renderer::Renderer() {
    mWhiteTexture = Texture::color(0xFF, 0xFF, 0xFF).build();

    mQuadVertexArray = VertexArray::create();
    mQuadVertexBuffer = VertexBuffer::builder()
      .size(QUAD_VERTEX_BUFFER_BYTE_SIZE)
      .storage(Buffer::StorageType::Dynamic)
      .layout(BufferElement::Type::Float2, "position")
      .layout(BufferElement::Type::Float4, "color")
      .layout(BufferElement::Type::Float2, "texture")
      .layout(BufferElement::Type::Uint, "texIndex")
      .build();
    mQuadVertexArray->addVertexBuffer(mQuadVertexBuffer);

    u32* indices = new u32[QUAD_INDEX_BUFFER_COUNT];
    u32  offset = 0;
    for (u32 i = 0; i < QUAD_INDEX_BUFFER_COUNT; i += QUAD_INDICES_COUNT) {
      indices[0 + i] = 0 + offset;
      indices[1 + i] = 1 + offset;
      indices[2 + i] = 2 + offset;

      indices[3 + i] = 2 + offset;
      indices[4 + i] = 3 + offset;
      indices[5 + i] = 0 + offset;

      offset += QUAD_VERTICES_COUNT;
    }

    mQuadIndexBuffer = IndexBuffer::create({indices, QUAD_INDEX_BUFFER_COUNT});
    delete[] indices;
    mQuadVertexArray->setIndexBuffer(mQuadIndexBuffer);
    mQuadVertexArray->unbind();

    mQuadShader = Shader::load("assets/shaders/renderer/Quad.glsl").build();

    mQuadBasePtr = new QuadVertex[QUAD_MAX * QUAD_VERTICES_COUNT];
    mQuadCurrentPtr = mQuadBasePtr;
    mQuadCount = 0;

    mQuadTextures.reserve(MAX_TEXTURES);
    mQuadTextures.push_back(mWhiteTexture);

    i32 samples[MAX_TEXTURES];
    for (u32 i = 0; i < Renderer::MAX_TEXTURES; ++i) {
      samples[i] = i;
    }
    mQuadShader->bind();
    mQuadShader->setIntArray("uTextures", samples, MAX_TEXTURES);

    Logger::warn("hEllo");
    fflush(stdout);
    mFontTexture = Texture::load("assets/textures/PixelFont_7x9_112x54.png")
      .filtering(Texture::FilteringMode::Nearest)
      .mipmap(Texture::MipmapMode::None)
      .build();

    mFontTextureWidth  = mFontTexture->getWidth();
    mFontTextureHeight = mFontTexture->getHeight();
    mFontCharacterWidth  = 7;
    mFontCharacterHeight = 9;

    GATE_DEBUG_ASSERT(mFontTextureWidth % mFontCharacterWidth == 0);
    GATE_DEBUG_ASSERT(mFontTextureHeight % mFontCharacterHeight == 0);

    u32 count = 0;
    for (u32 height = 0; height != mFontTextureHeight; height += mFontCharacterHeight) {
      for (u32 width = 0; width != mFontTextureWidth; width += mFontCharacterWidth) {
        const f32 x     = (f32)width  / mFontTextureWidth;
        const f32 y     = (f32)height / mFontTextureHeight;
        const f32 xSize = (f32)mFontCharacterWidth / mFontTextureWidth;
        const f32 ySize = (f32)mFontCharacterHeight / mFontTextureHeight;
        mFontCoords[count++] = Vec4{
          Vec2{ x,         1 - y - ySize },
          Vec2{ x + xSize, 1 - y - ySize / mFontCharacterHeight },
        };
      }
    }
  }

  Renderer::~Renderer() {
    delete[] mQuadBasePtr;
  }

  void Renderer::begin(const Camera& camera) {
    mProjectionViewMatrix = camera.getProjectionViewMatrix();
  }

  void Renderer::drawChar(char c, const Vec2& position, const Vec2& size, const Vec4& color) {
    Mat4 transform = Mat4(1.0f);
    transform      = glm::translate(transform, Vec3(position, 0.0f));
    transform      = glm::scale(transform, Vec3(size, 1.0f));

    auto texture = mFontTexture;

    // TODO: refactor this.
    if (mQuadCount == QUAD_MAX) {
      flush();
    }

    u32 index = 0;
    for (; index < mQuadTextures.size(); ++index) {
      if (mQuadTextures[index] == texture) {
        break;
      }
    }

    if (index == mQuadTextures.size()) {
      if (mQuadTextures.size() >= MAX_TEXTURES) {
        flush();
      }

      index = (u32)mQuadTextures.size();
      mQuadTextures.push_back(texture);
    }

    if (!std::isprint(c)) {
      c = (usize)('~' + 1);
    }

    Vec4 tc = mFontCoords[(usize)(c - ' ')];

    // TODO: do transfrom with projection view matrix here
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[0]), color, /* {1.0f, 1.0f} */ {tc.z, tc.w}, index }; // top-right
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[1]), color, /* {1.0f, 0.0f} */ {tc.z, tc.y}, index }; // bottom-right
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[2]), color, /* {0.0f, 0.0f} */ {tc.x, tc.y}, index }; // bottom-left
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[3]), color, /* {0.0f, 1.0f} */ {tc.x, tc.w}, index }; // top-left

    mQuadCount++;
  }

  void Renderer::drawText(const StringView& text, const Vec2& position, const float _size, const Vec4& color) {
    Vec2 size = {_size - _size/7.0f, _size};

    Vec2 start = position;
    Vec2 current = start;
    for (const char c : text) {
      if (c != '\n') {
        drawChar(c, current, size, color);
        current.x += size.x;
      } else {
        current.y -= size.y; 
        current.x  = start.x;
      }
    }
  }

  void Renderer::drawCenteredQuad(const Vec2& position, const Vec2& size, const Vec4& color) {
    drawQuad(position - size / 2.0f, size, color);
  }

  void Renderer::drawCenteredQuad(const Vec2& position, const Vec2& size, const Texture::Handle& texture, const Vec4& color) {
    drawQuad(position - size / 2.0f, size, texture, color);
  }
  
  void Renderer::drawQuad(const Vec2& position, const Vec2& size, const Vec4& color) {
    Renderer::drawQuad(position, size, mWhiteTexture, color);
  }

  void Renderer::drawQuad(const Vec2& position, const Vec2& size, const Texture::Handle& texture, const Vec4& color) {
    Mat4 transform = Mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));  

    // transform = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); 
    // transform = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); 
    // transform = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    transform = glm::scale(transform, glm::vec3(size, 1.0f)); 

    if (mQuadCount >= QUAD_MAX) {
      flush();
    }

    u32 index = 0;
    for (; index < mQuadTextures.size(); ++index) {
      if (mQuadTextures[index] == texture) {
        break;
      }
    }

    if (index == mQuadTextures.size()) {
      if (mQuadTextures.size() >= MAX_TEXTURES) {
        flush();
      }

      index = (u32)mQuadTextures.size();
      mQuadTextures.push_back(texture);
    }

    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[0]), color, {1.0f, 1.0f}, index }; // top-right
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[1]), color, {1.0f, 0.0f}, index }; // bottom-right
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[2]), color, {0.0f, 0.0f}, index }; // bottom-left
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[3]), color, {0.0f, 1.0f}, index }; // top-left

    mQuadCount++;
  }

  void Renderer::clearScreen(const Vec4& color) {
    clearScreen(mWhiteTexture, color);
  }

  void Renderer::clearScreen(const Texture::Handle& texture, const Vec4& color) {
    drawQuad(Vec2{0, 0}, Vec2{Application::getWindow().getWidth(), Application::getWindow().getHeight()}, texture, color);
  }

  void Renderer::flush() {
    if (mQuadCount) {
      mWhiteTexture->bind();

      for (u32 i = 0; i < mQuadTextures.size(); ++i) {
        mQuadTextures[i]->bind(i);
      }

      mQuadShader->bind();

      mQuadVertexArray->bind();
      mQuadVertexBuffer->set({mQuadBasePtr,  mQuadCount * QUAD_VERTICES_COUNT});
      mQuadVertexArray->drawIndices(mQuadCount * QUAD_INDICES_COUNT);

      mQuadCurrentPtr = mQuadBasePtr;
      mQuadCount = 0;

      mQuadTextures.clear();
      mQuadTextures.push_back(mWhiteTexture);
    }
  }

  void Renderer::end() {
    flush();
  }

  void Renderer::blending(bool yes) {
    if (mBlending == yes) {
      return;
    }

    flush();

    mBlending = yes;
    if (mBlending) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    } else {
      glDisable(GL_BLEND);
    }
  }

} // namespace Gate
