#include "Editor/Components/Component.hpp"
#include "Editor/Config.hpp"

#include "Application.hpp"

namespace Gate {

  Component::~Component() {}

  void Component::renderConnectors(Renderer& renderer) {
    // TODO: Show when outputs and inputs are active
    for (auto& pin : mInputPins) {
      renderer.drawCenteredQuad(
        pin.position.toVec2() * (f32)config.grid.cell.size,
        config.component.input.size,
        config.component.input.color
      );
    }
    for (auto& pin : mOutputPins) {
      renderer.drawCenteredQuad(
        pin.position.toVec2() * (f32)config.grid.cell.size,
        config.component.output.size,
        config.component.output.color
      );
    }
  }
  void Component::resetVisited() {
    setVisited(false);
    for (auto& pin : mInputPins) {
      pin.visited = false;
    }
    for (auto& pin : mOutputPins) {
      pin.visited = false;
    }
  }
  bool Component::areAllInputPinsVisited() const {
    for (auto& pin : mInputPins) {
      if (!pin.visited) {
        return false;
      }
    }
    return true;
  }

}