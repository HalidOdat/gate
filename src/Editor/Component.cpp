#include "Editor/Component.hpp"

namespace Gate {

  void Component::render(Renderer& renderer, u32 gridSize) {
    Vec2 size = Vec2{(f32)gridSize};
    renderer.drawQuad(size * 2.0f, size * 2.0f, Color::RED);
  }

}