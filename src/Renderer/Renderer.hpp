#pragma once

#include "Renderer/Camera.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Texture.hpp"
#include "Core/Math.hpp"
#include "Utils/Color.hpp"

namespace Game {
  
  class Renderer {
  public:
    Renderer() = delete;

    static void Initialize();
    static void Shutdown();
    static void begin(const Camera& camera);
    static void drawQuad(const Vec2& position, const Vec2& size, const Vec4& color = Color::WHITE);
    static void draw(const Ref<Shader>& shader, const Ref<VertexArray>& vao, const Ref<Texture2D>& texture);
    static void end();
  };

} // namespace Game
