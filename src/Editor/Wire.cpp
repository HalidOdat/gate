#include "Editor/Wire.hpp"
#include "Editor/Config.hpp"

namespace Gate {
  
  void Wire::render(Renderer& renderer) {
    f32 wireWidth = config.wire.width;
    Vec2 size = to.toVec2();
    if (from.x == to.x) {
      size.x  = wireWidth;
      size.y += wireWidth;
    } else {
      size.x += wireWidth;
      size.y = wireWidth;
    }

    Vec4 color = config.wire.inactiveColor;
    if (active) {
      color = config.wire.activeColor;
    }

    renderer.drawQuad(
      from.toVec2() - Vec2(wireWidth / 2.0f),
      size,
      color
    );
  }

}