#pragma once

#include "Core/Math.hpp"
#include "Utils/Color.hpp"
#include "Resource/Shader.hpp"
#include "Resource/Texture.hpp"
#include "Resource/Mesh.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Camera.hpp"

namespace Game {
  
  class Application;

  class Renderer {
    friend class Application;

  public:
    Renderer() = delete;

    static void enableBlending(bool  yes = true);
    static void enableDepthTest(bool yes = true);
    static void enableCullFace(bool  yes = true);

    static void begin(const Camera& camera);
    static void drawQuad(const Vec2& position,  const Vec2& size, const Vec4& color = Color::WHITE);
    static void drawQuad(const Vec3& position,  const Vec2& size, const Vec4& color = Color::WHITE);
    static void drawQuad(const Vec3& position,  const Vec2& size, const Texture2D& texture, const Vec4& color = Color::WHITE);
    static void drawQuad(const Mat4& transform, const Texture2D& texture, const Vec4& color = Color::WHITE);

    static void drawChar(char c, const Vec3& position,  const Vec2& size, const Vec4& color = Color::WHITE);
    static void drawChar(char c, const Mat4& transform, const Vec4& color = Color::WHITE);
    static void drawText(const StringView& text, const Vec3& position, const Vec2& size, const Vec4& color = Color::WHITE);
    static void drawText(const StringView& text, const Vec2& position, const float size, const Vec4& color = Color::WHITE);

    static void submit(const Ref<Shader>& shader, const Ref<VertexArray>& vao, const Ref<Texture2D>& texture);
    static void submit(Shader& shader, const Mesh& mesh, const Mat4& transform = Mat4(1.0f));

    static void render();

    static void flush();
    static void end();


  private:
    static void Initialize();
    static void Shutdown();
  };

} // namespace Game
