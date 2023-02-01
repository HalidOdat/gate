#include "Editor/Components/Component.hpp"
#include "Editor/Config.hpp"

#include "Application.hpp"

namespace Gate {

  Component::~Component() {}

  void Component::renderConnectors(Renderer& renderer) {
    // TODO: Show when outputs and inputs are active
    for (auto& pin : mPins) {
      Vec2 size;
      Vec4 color;
      switch (pin.type) {
        case Pin::Type::Input:
          size = config.component.input.size;
          color = config.component.input.color;
          break;
        case Pin::Type::Output:
          size = config.component.output.size;
          color = config.component.output.color;
          break;
      }
      renderer.drawCenteredQuad(
        pin.position.toVec2() * (f32)config.grid.cell.size,
        size,
        color
      );
    }
  }
  void Component::resetVisited() {
    setVisited(false);
    for (auto& pin : mPins) {
      pin.visited = false;
    }
  }
  bool Component::areAllInputPinsVisited() const {
    for (auto& pin : mPins) {
      if (pin.type == Pin::Type::Input && !pin.visited) {
        return false;
      }
    }
    return true;
  }

}