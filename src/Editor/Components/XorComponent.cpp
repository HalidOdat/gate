#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

namespace Gate {

  XorComponent::XorComponent(Point position)
    : Component(Component::Category::Gate, position)
  {
    this->mPins.push_back(Pin{Pin::Type::Input,  Point{position.x - 1, position.y - 1}});
    this->mPins.push_back(Pin{Pin::Type::Input,  Point{position.x - 1, position.y + 1}});
    this->mPins.push_back(Pin{Pin::Type::Output, Point{position.x + 1, position.y}});
  }
  void XorComponent::renderBody(Renderer& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    Vec4 color = Color::BLACK;
    if (mPins[OUTPUT_INDEX].active) {
      color = Color::RED;
    }
    renderer.drawCenteredQuad(mPosition.toVec2() * (f32)config.grid.cell.size, size * 2.5f, color);

    f32 fontSize = size.x * 1.5f;
    renderer.drawText("X", (mPosition.toVec2() * (f32)config.grid.cell.size) - fontSize / 2.0f, fontSize, Color::PURPLE);
  }
  bool XorComponent::update() {
    mPins[OUTPUT_INDEX].active = mPins[A_INPUT_INDEX].active != mPins[B_INPUT_INDEX].active;
    return true;
  }

}