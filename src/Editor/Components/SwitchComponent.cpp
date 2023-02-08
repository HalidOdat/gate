#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

namespace Gate {

  SwitchComponent::SwitchComponent(Point position)
    : Component(Component::Category::Input, position)
  {
    this->mOutputPins.push_back(Pin{Point{position.x + 1, position.y}});
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
    f32 size = config.grid.cell.size3d;

    Material::Handle material = config.inactiveMaterial;
    if (mOutputPins[OUTPUT_INDEX].active) {
      material = config.activeMaterial;
    }

    Mat4 model{1.0f};
    model = glm::translate(model, mPosition.toVec3() * config.grid.cell.size3d * Vec3{1.0f, -1.0f, 1.0f});
    size = size * 1.5f;
    model = glm::scale(model, Vec3(Vec2(size * 1.5f), size / 2.0f));

    renderer.submit(config.pinMesh, material, model, id);
  }

  GATE_COMPONENT_IMPLEMENTATION(SwitchComponent)

}