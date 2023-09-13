#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

namespace Gate {

  NotComponent::NotComponent(Point position)
    : Component(Component::Category::Gate, Type::NotGate, position)
  {
    this->mInputPins.push_back(Pin{Point{position.x - 1, position.y}});
    this->mOutputPins.push_back(Pin{Point{position.x + 1, position.y}});
  }
  void NotComponent::renderBody(Renderer2D& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    Vec4 color = Color::BLACK;
    if (mOutputPins[OUTPUT_INDEX].active) {
      color = Color::RED;
    }
    renderer.drawCenteredQuad(mPosition.toVec2() * (f32)config.grid.cell.size, size * 1.9f, config.notGate, color);
  }
  bool NotComponent::update() {
    mOutputPins[OUTPUT_INDEX].active = !mInputPins[INPUT_INDEX].active;
    return true;
  }

  void NotComponent::renderBody(Renderer3D& renderer, u32 id) {
    Material::Handle material = config.inactiveMaterial;
    if (mOutputPins[OUTPUT_INDEX].active) {
      material = config.activeMaterial;
    }

    const auto model = Component::computeModel(1.85f);

    renderer.submit(config.notMesh, material, model, id);
  }

  GATE_COMPONENT_IMPLEMENTATION(NotComponent)

}
