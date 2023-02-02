#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

namespace Gate {

  NotComponent::NotComponent(Point position)
    : Component(Component::Category::Gate, position)
  {
    this->mInputPins.push_back(Pin{Point{position.x - 1, position.y}});
    this->mOutputPins.push_back(Pin{Point{position.x + 1, position.y}});
  }
  void NotComponent::renderBody(Renderer& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    Vec4 color = Color::BLACK;
    if (mOutputPins[OUTPUT_INDEX].active) {
      color = Color::RED;
    }
    renderer.drawCenteredQuad(mPosition.toVec2() * (f32)config.grid.cell.size, size * 1.5f, color);
  }
  bool NotComponent::update() {
    mOutputPins[OUTPUT_INDEX].active = !mInputPins[INPUT_INDEX].active;
    return true;
  }

}