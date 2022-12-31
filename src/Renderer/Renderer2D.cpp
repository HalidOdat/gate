#include <array>
#include <cctype>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Base.hpp"
#include "Application.hpp"

#include "Renderer/Renderer2D.hpp"

namespace Game {

  Renderer2D::Renderer2D() {
    GAME_PROFILE_FUNCTION();

    mWhiteTexture = Texture2D::color(0xFF, 0xFF, 0xFF).build();

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
    for (u32 i = 0; i < Renderer2D::MAX_TEXTURES; ++i) {
      samples[i] = i;
    }
    mQuadShader->bind();
    mQuadShader->setIntArray("uTextures", samples, MAX_TEXTURES);

    mFontTexture = Texture2D::load("assets/textures/PixelFont_7x9_112x54.png")
      .filtering(Texture::FilteringMode::Nearest)
      .mipmap(Texture::MipmapMode::None)
      .build();

    mFontTextureWidth  = mFontTexture->getWidth();
    mFontTextureHeight = mFontTexture->getHeight();
    mFontCharacterWidth  = 7;
    mFontCharacterHeight = 9;

    GAME_DEBUG_ASSERT(mFontTextureWidth % mFontCharacterWidth == 0);
    GAME_DEBUG_ASSERT(mFontTextureHeight % mFontCharacterHeight == 0);

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

  Renderer2D::~Renderer2D() {
    GAME_PROFILE_FUNCTION();
    delete[] mQuadBasePtr;
  }

  void Renderer2D::begin(const Camera& camera) {
    GAME_PROFILE_FUNCTION();
    mProjectionViewMatrix = camera.getProjectionViewMatrix();
  }

  void Renderer2D::drawChar(char c, const Vec2& position, const Vec2& size, const Vec4& color) {
    GAME_PROFILE_FUNCTION();

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

  void Renderer2D::drawText(const StringView& text, const Vec2& position, const float _size, const Vec4& color) {
    GAME_PROFILE_FUNCTION();
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

  void Renderer2D::drawQuad(const Vec2& position, const Vec2& size, const Vec4& color) {
    Renderer2D::drawQuad(position, size, mWhiteTexture, color);
  }

  void Renderer2D::drawQuad(const Vec2& position, const Vec2& size, const Texture2D::Handle& texture, const Vec4& color) {
    GAME_PROFILE_FUNCTION();

    Mat4 transform = Mat4(1.0f);
    transform      = glm::translate(transform, Vec3(position, 1.0f));
    transform      = glm::scale(transform, Vec3(size, 1.0f));

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

    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[0]), color, {1.0f, 1.0f}, index }; // top-right
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[1]), color, {1.0f, 0.0f}, index }; // bottom-right
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[2]), color, {0.0f, 0.0f}, index }; // bottom-left
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[3]), color, {0.0f, 1.0f}, index }; // top-left

    mQuadCount++;
  }

  void Renderer2D::flush() {
    GAME_PROFILE_FUNCTION();
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

  void Renderer2D::end() {
    flush();
  }

} // namespace Game
