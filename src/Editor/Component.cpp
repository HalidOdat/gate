#include "Editor/Component.hpp"
#include "Editor/Config.hpp"

namespace Gate {

  void Component::render(Renderer& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    renderer.drawQuad(size * 2.0f, size * 2.0f, Color::RED);
  }

}