#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

namespace Gate {

  NotComponent::NotComponent(Point position)
    : Component(Component::Category::Gate, position)
  {
    this->mPins.push_back(Pin{Pin::Type::Input,  Point{position.x - 1, position.y}});
    this->mPins.push_back(Pin{Pin::Type::Output, Point{position.x + 1, position.y}});
  }
  void NotComponent::renderBody(Renderer& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    Vec4 color = Color::BLACK;
    if (mPins[OUTPUT_INDEX].active) {
      color = Color::RED;
    }
    renderer.drawCenteredQuad(mPosition.toVec2() * (f32)config.grid.cell.size, size * 1.5f, color);
  }
  bool NotComponent::update() {
    // if (mPins[INPUT_INDEX].visited) {
    //   return false;
    // }
    mPins[OUTPUT_INDEX].active = !mPins[INPUT_INDEX].active;
    return true;
  }

}