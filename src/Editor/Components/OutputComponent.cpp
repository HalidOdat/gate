#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

namespace Gate {

  OutputComponent::OutputComponent(Point position)
    : Component(Component::Category::Gate, Type::Output, position)
  {
    this->mInputPins.push_back(Pin{Point{position.x - 1, position.y}});
  }
  void OutputComponent::renderBody(Renderer2D& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    Vec4 color = Color::BLACK;
    if (mInputPins[INPUT_INDEX].active) {
      color = Color::RED;
    }
    renderer.drawCenteredQuad(mPosition.toVec2() * (f32)config.grid.cell.size, size * 1.5f, color);
  }

  bool OutputComponent::update() {
    return true;
  }

  void OutputComponent::renderBody(Renderer3D& renderer, u32 id) {
    Material::Handle material = config.inactiveMaterial;
    if (mInputPins[INPUT_INDEX].active) {
      material = config.activeMaterial;
    }

    const auto model = Component::computeModel(1.5f);

    renderer.submit(config.pinMesh, material, model, id);
  }

  GATE_COMPONENT_IMPLEMENTATION(OutputComponent)

}