#include "Editor/Wire.hpp"

namespace Gate {
  
  void Wire::render(Renderer& renderer, f32 wireWidth) {
    Vec2 size = to.toVec2();
    if (from.x == to.x) {
      size.x  = 5;
      size.y += wireWidth;
    } else {
      size.x += wireWidth;
      size.y = 5;
    }

    Vec4 color = Color::BLACK;
    if (active) {
      color = Color::RED;
    }

    renderer.drawQuad(
      from.toVec2() - Vec2(wireWidth / 2.0f),
      size,
      color
    );
  }

}