#include "Editor/Wire.hpp"
#include "Editor/Config.hpp"

namespace Gate {
  
  void Wire::render(Renderer& renderer) {
    f32 width = config.wire.width;

    Vec2 size = (to.toVec2() - from.toVec2()) * (f32)config.grid.cell.size;
    if (from.x == to.x) {
      size.x  = width;
      size.y += width;
    } else {
      size.x += width;
      size.y  = width;
    }

    Vec4 color = config.wire.inactiveColor;
    if (active) {
      color = config.wire.activeColor;
    }

    if (!visited) {
      color = config.wire.invalidColor;
    }

    renderer.drawQuad(
      from.toVec2() * (f32)config.grid.cell.size - Vec2(width / 2.0f),
      size,
      color
    );
    renderer.drawCenteredQuad(
      from.toVec2() * (f32)config.grid.cell.size,
      config.wire.endsSize,
      color
    );
    renderer.drawCenteredQuad(
      to.toVec2() * (f32)config.grid.cell.size,
      config.wire.endsSize,
      color
    );
  }

}