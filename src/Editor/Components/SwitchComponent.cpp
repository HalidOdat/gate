#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

namespace Gate {

  SwitchComponent::SwitchComponent(Point position)
    : Component(position)
  {
    this->mPins.push_back(Pin{Pin::Type::Output, Point{position.x + 1, position.y}});
  }
  void SwitchComponent::renderBody(Renderer& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    Vec4 color = Color::BLACK;
    if (mPins[OUTPUT_INDEX].active) {
      color = Color::RED;
    }
    renderer.drawCenteredQuad(mPosition.toVec2() * (f32)config.grid.cell.size, size * 1.5f, color);
  }

  void SwitchComponent::update() {}

  void SwitchComponent::click() {
    this->toggle();
  }

}