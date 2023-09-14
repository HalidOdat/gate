#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

namespace Gate {

  OrComponent::OrComponent(Point position)
     : Component(Component::Category::Gate, Type::OrGate, position)
  {
    this->mInputPins.push_back(Pin{Point{position.x - 1, position.y - 1}});
    this->mInputPins.push_back(Pin{Point{position.x - 1, position.y + 1}});
    this->mOutputPins.push_back(Pin{Point{position.x + 1, position.y}});
  }
  void OrComponent::renderBody(Renderer2D& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    Vec4 color = Color::BLACK;
    if (mOutputPins[OUTPUT_INDEX].active) {
      color = Color::RED;
    }
    renderer.drawCenteredQuad(mPosition.toVec2() * (f32)config.grid.cell.size, Vec2{size.x * 2.2f, size.y * 2.4f}, config.orGate, color);
  }
  bool OrComponent::update() {
    mOutputPins[OUTPUT_INDEX].active = mInputPins[A_INPUT_INDEX].active || mInputPins[B_INPUT_INDEX].active;
    return true;
  }

  void OrComponent::renderBody(Renderer3D& renderer, u32 id) {
    Material::Handle material = config.inactiveMaterial;
    if (mOutputPins[OUTPUT_INDEX].active) {
      material = config.activeMaterial;
    }

    const auto model = Component::computeModel(2.8f);

    renderer.submit(config.orMesh, material, model, id);
  }

  GATE_COMPONENT_IMPLEMENTATION(OrComponent)

}