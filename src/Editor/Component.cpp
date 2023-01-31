#include "Editor/Component.hpp"
#include "Editor/Config.hpp"

#include "Application.hpp"

namespace Gate {

  Component::~Component() {}

  SwitchComponent::SwitchComponent(Point position)
    : Component(position), mOut(position.x + 1, position.y)
  {}
  void SwitchComponent::renderBody(Renderer& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    renderer.drawCenteredQuad(mPosition.toVec2() * (f32)config.grid.cell.size, size * 1.5f, Color::RED);
  }

  void SwitchComponent::renderConnectors(Renderer& renderer) {
    renderer.drawCenteredQuad(
      mOut.toVec2() * (f32)config.grid.cell.size,
      // TODO: put in config connector theme and size
      config.component.output.size,
      config.component.output.color
    );
  }

}