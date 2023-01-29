#pragma once

#include "Core/Math.hpp"
#include "Utils/Color.hpp"
#include "Resource/Manager.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/CameraController.hpp"

#include <vector>
#include <array>

namespace Gate {
  
  class Application;

  class Renderer {
  public:
    Renderer();
    DISALLOW_MOVE_AND_COPY(Renderer);
    ~Renderer();

    void blending(bool yes = true);

    void begin(const Camera& camera);
    void end();

    void clearScreen(const Vec4& color = Color::WHITE);

    void drawQuad(const Vec2& position, const Vec2& size, const Texture::Handle& texture, const Vec4& color = Color::WHITE);
    void drawQuad(const Vec2& position, const Vec2& size, const Vec4& color = Color::WHITE);
    void drawChar(char c, const Vec2& position,  const Vec2& size, const Vec4& color = Color::WHITE);
    void drawText(const StringView& text, const Vec2& position, const float size, const Vec4& color = Color::WHITE);

    void flush();

  private:
    struct QuadVertex {
      Vec2 position;
      Vec4 color;
      Vec2 texCoord;
      u32  texIndex;
    };

  private:
    static constexpr const u32 QUAD_MAX              = 1024;
    static constexpr const u32 QUAD_VERTICES_COUNT   = 4;
    static constexpr const u32 QUAD_INDICES_COUNT    = 6;
    static constexpr const u32 QUAD_VERTEX_BUFFER_BYTE_SIZE = QUAD_MAX * QUAD_VERTICES_COUNT * sizeof(QuadVertex);
    static constexpr const u32 QUAD_INDEX_BUFFER_COUNT      = QUAD_MAX * QUAD_INDICES_COUNT;

    static constexpr const std::array<Vec4, 4> QUAD_POSITIONS = {
      Vec4{  1.0f,  0.0f, 0.0f, 1.0f }, // top    right
      Vec4{  1.0f,  1.0f, 0.0f, 1.0f }, // bottom right
      Vec4{  0.0f,  1.0f, 0.0f, 1.0f }, // bottom left
      Vec4{  0.0f,  0.0f, 0.0f, 1.0f }, // top    left 
    };

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

    // Font rendering
    Texture::Handle mFontTexture;
    u32 mFontTextureWidth;
    u32 mFontTextureHeight;
    u32 mFontCharacterWidth;
    u32 mFontCharacterHeight;
    std::array<Vec4, 96> mFontCoords;
  };

} // namespace Gate
