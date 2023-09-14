#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

namespace Gate {

  SwitchComponent::SwitchComponent(Point position)
    : Component(Component::Category::Input, Type::Switch, position)
  {
    this->mOutputPins.push_back(Pin{Point{position.x + 1, position.y}});
  }

  bool SwitchComponent::deletable() {
    return mDeletable;
  }
  void SwitchComponent::setDeletable(bool value) {
    mDeletable = value;
  }

  void SwitchComponent::renderBody(Renderer2D& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    Vec4 color = Color::BLACK;
    if (mOutputPins[OUTPUT_INDEX].active) {
      color = Color::RED;
    }
    renderer.drawCenteredQuad(mPosition.toVec2() * (f32)config.grid.cell.size, size * 1.5f, color);
  }
  bool SwitchComponent::update() {
    return true;
  }
  void SwitchComponent::click() {
    this->toggle();
  }

  void SwitchComponent::renderBody(Renderer3D& renderer, u32 id) {
    Material::Handle material = config.inactiveMaterial;
    if (mOutputPins[OUTPUT_INDEX].active) {
      material = config.activeMaterial;
    }

    const auto model = Component::computeModel(1.5f);

    renderer.submit(config.pinMesh, material, model, id);
  }

  GATE_COMPONENT_IMPLEMENTATION(SwitchComponent)

}