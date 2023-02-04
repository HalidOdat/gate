#include "Editor/Pin.hpp"

#include "Editor/Config.hpp"

namespace Gate {

  void Pin::render(Renderer2D& renderer, bool isOutput) {
    renderer.drawCenteredCircle(
      position.toVec2() * (f32)config.grid.cell.size,
      (isOutput ? config.component.output.size.x : config.component.input.size.x) / 2.0f,
      isOutput ? config.component.output.color : config.component.input.color
    );
    renderer.drawCenteredCircle(
      position.toVec2() * (f32)config.grid.cell.size,
      (isOutput ? config.component.output.size.x : config.component.input.size.x) / 2.0f,
      Color::BLUE,
      0.2f, // thickness
      0.001f
    );
  }

}