#include <array>
#include <cctype>

#include "Core/OpenGL.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Base.hpp"
#include "Application.hpp"

#include "Renderer/Renderer2D.hpp"

namespace Gate {

  Renderer2D::Renderer2D() {
    mWhiteTexture = Texture::color(0xFF, 0xFF, 0xFF).build();

    mQuadVertexArray = VertexArray::create();
    mQuadVertexBuffer = VertexBuffer::builder()
      .size(QUAD_VERTEX_BUFFER_BYTE_SIZE)
      .storage(Buffer::StorageType::Dynamic)
      .layout(BufferElement::Type::Float2) // position
      .layout(BufferElement::Type::Float4) // color
      .layout(BufferElement::Type::Float2) // texture
      .layout(BufferElement::Type::Uint)   // texIndex
      .layout(BufferElement::Type::Uint)   // aEffectMode
      .layout(BufferElement::Type::Float2) // aQuadSize
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

    mQuadShader = Shader::load("assets/2D/shaders/Quad.glsl").build();
    mQuadShader->bind();
    mQuadShader->setVec2("uResolution", Vec2{Application::getWindow().getWidth(), Application::getWindow().getHeight()});

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

    // Circles
    {
      mCircleVertexArray = VertexArray::create();
      mCircleVertexBuffer = VertexBuffer::builder()
        .size(CIRCLE_VERTEX_BUFFER_BYTE_SIZE)
        .storage(Buffer::StorageType::Dynamic)
        .layout(BufferElement::Type::Float2) // aWorldPosition
        .layout(BufferElement::Type::Float2) // aLocalPosition
        .layout(BufferElement::Type::Float4) // aColor
        .layout(BufferElement::Type::Float)  // aThickness
        .layout(BufferElement::Type::Float)  // aFade
        .build();
      mCircleVertexArray->addVertexBuffer(mCircleVertexBuffer);
      mCircleVertexArray->setIndexBuffer(mQuadIndexBuffer); // Use quad index buffer
      mCircleBasePtr = new CircleVertex[CIRCLE_MAX * CIRCLE_VERTICES_COUNT];
      mCircleCurrentPtr = mCircleBasePtr;
      mCircleCount = 0;
      
      mCircleShader = Shader::load("assets/2D/shaders/Circle.glsl").build();
    }

    auto fontTexture = Texture::load("assets/2D/textures/PixelFont_7x9_112x54.png")
      .filtering(Texture::FilteringMode::Nearest)
      .mipmap(Texture::MipmapMode::None)
      .build();

    const auto fontTextureWidth  = fontTexture->getWidth();
    const auto fontTextureHeight = fontTexture->getHeight();
    const auto fontCharacterWidth  = 7;
    const auto fontCharacterHeight = 9;

    GATE_DEBUG_ASSERT(fontTextureWidth % fontCharacterWidth == 0);
    GATE_DEBUG_ASSERT(fontTextureHeight % fontCharacterHeight == 0);

    mFontAtlas = TextureAtlas(fontTexture, fontCharacterWidth, fontCharacterHeight);
  }

  Renderer2D::~Renderer2D() {
    delete[] mQuadBasePtr;
    delete[] mCircleBasePtr;
  }

  void Renderer2D::invalidate(u32 width, u32 height) {
    mQuadShader->bind();
    mQuadShader->setVec2("uResolution", Vec2{width, height});
  }

  void Renderer2D::begin(const Camera& camera) {
    mProjectionViewMatrix = camera.getProjectionViewMatrix();
  }

  void Renderer2D::drawChar(char c, const Vec2& position, const Vec2& size, const Vec4& color, Effect effect) {
    if (!std::isprint(c)) {
      c = (usize)('~' + 1);
    }

    auto texture = mFontAtlas.get(u32(c - ' '));
    drawQuad(position, size, texture, color, effect);
  }

  void Renderer2D::drawText(const StringView& text, const Vec2& position, const float _size, const Vec4& color, Effect effect) {
    Vec2 size = {_size - _size/7.0f, _size};

    Vec2 start = position;
    Vec2 current = start;
    for (const char c : text) {
      if (c != '\n') {
        drawChar(c, current, size, color, effect);
        current.x += size.x;
      } else {
        current.y += size.y; 
        current.x  = start.x;
      }
    }
  }

  void Renderer2D::drawCenteredQuad(const Vec2& position, const Vec2& size, const Vec4& color, Effect effect) {
    drawQuad(position - size / 2.0f, size, color, effect);
  }

  void Renderer2D::drawCenteredQuad(const Vec2& position, const Vec2& size, const SubTexture& texture, const Vec4& color, Effect effect) {
    drawQuad(position - size / 2.0f, size, texture, color, effect);
  }
  
  void Renderer2D::drawQuad(const Vec2& position, const Vec2& size, const Vec4& color, Effect effect) {
    Renderer2D::drawQuad(position, size, mWhiteTexture, color, effect);
  }

  void Renderer2D::drawQuad(const Vec2& position, const Vec2& size, const SubTexture& texture, const Vec4& color, Effect effect) {
    Mat4 transform = Mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));  

    // transform = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); 
    // transform = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); 
    // transform = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    transform = glm::scale(transform, glm::vec3(size, 1.0f)); 

    if (mQuadCount >= QUAD_MAX) {
      flushQuad();
    }

    u32 index = 0;
    for (; index < mQuadTextures.size(); ++index) {
      if (mQuadTextures[index] == texture.getTexture()) {
        break;
      }
    }

    if (index == mQuadTextures.size()) {
      if (mQuadTextures.size() >= MAX_TEXTURES) {
        flush();
      }

      index = (u32)mQuadTextures.size();
      mQuadTextures.push_back(texture.getTexture());
    }

    const auto from = texture.getFrom();
    const auto to = texture.getTo();

    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[0]), color, {to.x,     to.y}, index, effect.toIndex(), size}; // top-right
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[1]), color, {to.x,   from.y}, index, effect.toIndex(), size}; // bottom-right
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[2]), color, {from.x, from.y}, index, effect.toIndex(), size}; // bottom-left
    *(mQuadCurrentPtr++) = { Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[3]), color, {from.x,   to.y}, index, effect.toIndex(), size}; // top-left

    mQuadCount++;
  }

  void Renderer2D::clearScreen(const Vec4& color) {
    clearScreen(mWhiteTexture, color);
  }

  void Renderer2D::clearScreen(const Texture::Handle& texture, const Vec4& color) {
    drawQuad(Vec2{0, 0}, Vec2{Application::getWindow().getWidth(), Application::getWindow().getHeight()}, texture, color);
  }
  void Renderer2D::flushQuad() {
    if (mQuadCount) {
      mWhiteTexture->bind();

      for (u32 i = 0; i < mQuadTextures.size(); ++i) {
        mQuadTextures[i]->bind(i);
      }

      mQuadShader->bind();
      mQuadShader->setFloat("uTime", (f32)glfwGetTime());

      mQuadVertexArray->bind();
      mQuadVertexBuffer->set({mQuadBasePtr,  mQuadCount * QUAD_VERTICES_COUNT});
      mQuadVertexArray->drawIndices(mQuadCount * QUAD_INDICES_COUNT);

      mQuadCurrentPtr = mQuadBasePtr;
      mQuadCount = 0;

      mQuadTextures.clear();
      mQuadTextures.push_back(mWhiteTexture);
    }
  }
  void Renderer2D::drawCenteredCircle(const Vec2& position, float radius, const Vec4& color, float thickness, float fade) {
    drawCircle(position - radius, radius, color, thickness, fade);
  }
  void Renderer2D::drawCircle(const Vec2& position, float radius, const Vec4& color, float thickness, float fade) {
    Vec3 scale = Vec3(radius * 2.0f, radius * 2.0f, 1.0f);

    Mat4 transform = Mat4(1.0f);
    transform = glm::translate(transform, Vec3(position, 0.0f));
    transform = glm::scale(transform, scale);
    drawCircle(transform, color, thickness, fade);
  }
  void Renderer2D::drawCircle(const Mat4& transform, const Vec4& color, float thickness, float fade) {
		if (mCircleCount >= CIRCLE_MAX) {
      flushCircle();
    }

    const constexpr Vec2 localPositions[] = {
      Vec2{1.0f, 1.0f} * 2.0f - 1.0f,
		  Vec2{1.0f, 0.0f} * 2.0f - 1.0f,
		  Vec2{0.0f, 0.0f} * 2.0f - 1.0f,
		  Vec2{0.0f, 1.0f} * 2.0f - 1.0f,
    };

		for (size_t i = 0; i < CIRCLE_VERTICES_COUNT; i++) {
			mCircleCurrentPtr->worldPosition = Vec2(mProjectionViewMatrix * transform * QUAD_POSITIONS[i]);
			mCircleCurrentPtr->localPosition = localPositions[i];
			mCircleCurrentPtr->color         = color;
			mCircleCurrentPtr->thickness     = thickness;
			mCircleCurrentPtr->fade          = fade;
			mCircleCurrentPtr++;
		}
		mCircleCount++;
	}
  void Renderer2D::flushCircle() {
    if (mCircleCount) {
      // mWhiteTexture->bind();

      for (u32 i = 0; i < mCircleTextures.size(); ++i) {
        mCircleTextures[i]->bind(i);
      }

      mCircleShader->bind();

      mCircleVertexArray->bind();
      mCircleVertexBuffer->set({mCircleBasePtr,  mCircleCount * CIRCLE_VERTICES_COUNT});
      mCircleVertexArray->drawIndices(mCircleCount * CIRCLE_INDICES_COUNT);

      mCircleCurrentPtr = mCircleBasePtr;
      mCircleCount = 0;

      // mCircleTextures.clear();
      // mCircleTextures.push_back(mWhiteTexture);
    }
  }
  void Renderer2D::flush() {
    flushQuad();
    flushCircle();
  }
  void Renderer2D::end() {
    flush();
  }

  void Renderer2D::blending(bool yes) {
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
