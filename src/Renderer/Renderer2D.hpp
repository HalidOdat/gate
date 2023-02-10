#pragma once

#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/CameraController.hpp"

#include <array>

namespace Gate {

  class Effect {
  public:
    enum class Type {
      None    = 0,
      Striped = 1,
      Static  = 2,
      Rounded = 3,
    };

  public:
    Effect(Type type)
      : mType{type}
    {}

    u32 toIndex() const {
      return u32(mType);
    }

  private:
    Type mType;
  };

  class Renderer2D {
    friend class Application;
  public:
    Renderer2D();
    DISALLOW_MOVE_AND_COPY(Renderer2D);
    ~Renderer2D();

    void blending(bool yes = true);

    void begin(const Camera& camera);
    void end();

    void drawCenteredCircle(const Vec2& position, float radius, const Vec4& color, float thickness = 1.0f, float fade = 0.005f);
    void drawCircle(const Vec2& position, float radius, const Vec4& color, float thickness = 1.0f, float fade = 0.005f);
    void drawCircle(const Mat4& transform, const Vec4& color, float thickness = 1.0f, float fade = 0.005f);

    void clearScreen(const Vec4& color = Color::WHITE);
    void clearScreen(const Texture::Handle& texture, const Vec4& color = Color::WHITE);

    void drawCenteredQuad(const Vec2& position, const Vec2& size, const Vec4& color = Color::WHITE, Effect effect = Effect::Type::None);
    void drawCenteredQuad(const Vec2& position, const Vec2& size, const SubTexture& texture, const Vec4& color = Color::WHITE, Effect effect = Effect::Type::None);
    void drawQuad(const Vec2& position, const Vec2& size, const SubTexture& texture, const Vec4& color = Color::WHITE, Effect effect = Effect::Type::None);
    void drawQuad(const Vec2& position, const Vec2& size, const Vec4& color = Color::WHITE, Effect effect = Effect::Type::None);
    void drawChar(char c, const Vec2& position,  const Vec2& size, const Vec4& color = Color::WHITE, Effect effect = Effect::Type::None);
    void drawText(const StringView& text, const Vec2& position, const float size, const Vec4& color = Color::WHITE, Effect effect = Effect::Type::None);

    void flushCircle();
    void flushQuad();
    void flush();

  private:
    void invalidate(u32 width, u32 height);

  private:
    struct QuadVertex {
      Vec2 position;
      Vec4 color;
      Vec2 texCoord;
      u32  texIndex;
      u32  mode;
      Vec2 quadSize;
    };

    struct CircleVertex {
		  Vec2 worldPosition;
		  Vec2 localPosition;
		  Vec4 color;
		  float thickness;
		  float fade;
    };

  private:
    static constexpr const std::array<Vec4, 4> QUAD_POSITIONS = {
      Vec4{ 1.0f,  0.0f, 0.0f, 1.0f }, // top    right
      Vec4{ 1.0f,  1.0f, 0.0f, 1.0f }, // bottom right
      Vec4{ 0.0f,  1.0f, 0.0f, 1.0f }, // bottom left
      Vec4{ 0.0f,  0.0f, 0.0f, 1.0f }, // top    left 
    };

    static constexpr const u32 QUAD_MAX              = 512;
    static constexpr const u32 QUAD_VERTICES_COUNT   = 4;
    static constexpr const u32 QUAD_INDICES_COUNT    = 6;
    static constexpr const u32 QUAD_VERTEX_BUFFER_BYTE_SIZE = QUAD_MAX * QUAD_VERTICES_COUNT * sizeof(QuadVertex);
    static constexpr const u32 QUAD_INDEX_BUFFER_COUNT      = QUAD_MAX * QUAD_INDICES_COUNT;

    static constexpr const u32 CIRCLE_MAX              = QUAD_MAX;
    static constexpr const u32 CIRCLE_VERTICES_COUNT   = 4;
    static constexpr const u32 CIRCLE_INDICES_COUNT    = 6;
    static constexpr const u32 CIRCLE_VERTEX_BUFFER_BYTE_SIZE = CIRCLE_MAX * CIRCLE_VERTICES_COUNT * sizeof(CircleVertex);
    static constexpr const u32 CIRCLE_INDEX_BUFFER_COUNT      = CIRCLE_MAX * CIRCLE_INDICES_COUNT;

    static constexpr const u32 MAX_TEXTURES = 16;

  private:
    bool mBlending = false;

    // Camera
    Mat4 mProjectionViewMatrix;

    // Cached default white texture
    Texture::Handle mWhiteTexture;

    // Quad batching
    VertexArray::Handle  mQuadVertexArray;
    VertexBuffer::Handle mQuadVertexBuffer;
    IndexBuffer::Handle  mQuadIndexBuffer;
    Shader::Handle       mQuadShader;
    std::vector<Texture::Handle> mQuadTextures;
    QuadVertex* mQuadBasePtr = nullptr;
    QuadVertex* mQuadCurrentPtr = nullptr;
    u32 mQuadCount = 0;

    VertexArray::Handle  mCircleVertexArray;
    VertexBuffer::Handle mCircleVertexBuffer;
    Shader::Handle       mCircleShader;
    std::vector<Texture::Handle> mCircleTextures;
    CircleVertex* mCircleBasePtr = nullptr;
    CircleVertex* mCircleCurrentPtr = nullptr;
    u32 mCircleCount = 0;

    // Font rendering
    Texture::Handle mFontTexture;
    u32 mFontTextureWidth;
    u32 mFontTextureHeight;
    u32 mFontCharacterWidth;
    u32 mFontCharacterHeight;
    std::array<Vec4, 96> mFontCoords;
  };

} // namespace Gate
