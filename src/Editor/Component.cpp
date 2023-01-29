#include "Editor/Component.hpp"

namespace Gate {

  void Component::render(Renderer& renderer) {
    renderer.drawQuad({100, 100}, {100, 100}, Color::RED);
  }

}