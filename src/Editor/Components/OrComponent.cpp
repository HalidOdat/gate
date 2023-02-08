#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

namespace Gate {

  OrComponent::OrComponent(Point position)
    : Component(Component::Category::Gate, position)
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

    // f32 fontSize = size.x * 1.5f;
    // renderer.drawText("v", (mPosition.toVec2() * (f32)config.grid.cell.size) - fontSize / 2.0f, fontSize, Color::PURPLE);
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

    Mat4 model{1.0f};
    model = glm::translate(model, mPosition.toVec3() * config.grid.cell.size3d * Vec3{1.0f, -1.0f, 1.0f});
    
    f32 size = config.grid.cell.size3d * 2.5f;
    model = glm::scale(model, Vec3(Vec2(size), size / 2.0f));


    renderer.submit(config.pinMesh, material, model, id);
  }

  GATE_COMPONENT_IMPLEMENTATION(OrComponent)

}