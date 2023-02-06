#include "Editor/Components/Component.hpp"
#include "Editor/Config.hpp"

#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Gate {

  Component::~Component() {}

  void Component::renderConnectors(Renderer2D& renderer) {
    // TODO: Show when outputs and inputs are active
    for (auto& pin : mInputPins) {
      pin.render(renderer, false);
    }
    for (auto& pin : mOutputPins) {
      pin.render(renderer, true);
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

  void Component::renderConnectors(Renderer3D& renderer, u32 id) {
    for (auto& pin : mInputPins) {
      pin.render(renderer, false, id);
    }
    for (auto& pin : mOutputPins) {
      pin.render(renderer, true, id);
    }
  }

}