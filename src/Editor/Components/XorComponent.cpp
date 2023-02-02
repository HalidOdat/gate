#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

namespace Gate {

  XorComponent::XorComponent(Point position)
    : Component(Component::Category::Gate, position)
  {
    this->mInputPins.push_back(Pin{Point{position.x - 1, position.y - 1}});
    this->mInputPins.push_back(Pin{Point{position.x - 1, position.y + 1}});
    this->mOutputPins.push_back(Pin{Point{position.x + 1, position.y}});
  }
  void XorComponent::renderBody(Renderer& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    Vec4 color = Color::BLACK;
    if (mOutputPins[OUTPUT_INDEX].active) {
      color = Color::RED;
    }
    renderer.drawCenteredQuad(mPosition.toVec2() * (f32)config.grid.cell.size, size * 2.5f, color);

    f32 fontSize = size.x * 1.5f;
    renderer.drawText("X", (mPosition.toVec2() * (f32)config.grid.cell.size) - fontSize / 2.0f, fontSize, Color::PURPLE);
  }
  bool XorComponent::update() {
    mOutputPins[OUTPUT_INDEX].active = mInputPins[A_INPUT_INDEX].active != mInputPins[B_INPUT_INDEX].active;
    return true;
  }

}