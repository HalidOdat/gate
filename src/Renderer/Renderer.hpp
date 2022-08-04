#pragma once

#include <functional>

#include "Core/Math.hpp"
#include "Utils/Color.hpp"
#include "Resource/Manager.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/CameraController.hpp"

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
    static void begin3D(const PerspectiveCameraController& cameraController);
    static void drawQuad(const Vec2& position,  const Vec2& size, const Vec4& color = Color::WHITE);
    static void drawQuad(const Vec3& position,  const Vec2& size, const Vec4& color = Color::WHITE);
    static void drawQuad(const Vec3& position,  const Vec2& size, const Texture2D::Handle& texture, const Vec4& color = Color::WHITE);
    static void drawQuad(const Mat4& transform, const Texture2D::Handle& texture, const Vec4& color = Color::WHITE);

    static void drawChar(char c, const Vec3& position,  const Vec2& size, const Vec4& color = Color::WHITE);
    static void drawChar(char c, const Mat4& transform, const Vec4& color = Color::WHITE);
    static void drawText(const StringView& text, const Vec3& position, const Vec2& size, const Vec4& color = Color::WHITE);
    static void drawText(const StringView& text, const Vec2& position, const float size, const Vec4& color = Color::WHITE);

    static void submit(Shader::Handle& shader, const Mesh::Handle& mesh, const Material& material, const Mat4& transform = Mat4(1.0f));

    static void flush();
    static void end();

  private:
    static void waitAndRender();
    static void initialize();
    static void shutdown();
    static void invalidate(u32 width, u32 height);
  };

} // namespace Game
