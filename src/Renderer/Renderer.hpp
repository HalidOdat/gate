#pragma once

#include "Renderer/Camera.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Texture.hpp"
#include "Core/Math.hpp"
#include "Utils/Color.hpp"

namespace Game {
  
  class Application;

  class Renderer {
    friend class Application;

  public:
    Renderer() = delete;

    static void begin(const Camera& camera);
    static void drawQuad(const Vec2& position, const Vec2& size, const Vec4& color = Color::WHITE);
    static void drawQuad(const Vec3& position, const Vec2& size, const Vec4& color = Color::WHITE);
    static void draw(const Ref<Shader>& shader, const Ref<VertexArray>& vao, const Ref<Texture2D>& texture);
    static void flush();
    static void end();

  private:
    static void Initialize();
    static void Shutdown();
  };

} // namespace Game