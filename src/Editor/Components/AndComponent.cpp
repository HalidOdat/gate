#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

namespace Gate {

  AndComponent::AndComponent(Point position)
    : Component(Component::Category::Gate, position)
  {
    this->mInputPins.push_back(Pin{Point{position.x - 1, position.y - 1}});
    this->mInputPins.push_back(Pin{Point{position.x - 1, position.y + 1}});
    this->mOutputPins.push_back(Pin{Point{position.x + 1, position.y}});
  }
  void AndComponent::renderBody(Renderer2D& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    Vec4 color = Color::BLACK;
    if (mOutputPins[OUTPUT_INDEX].active) {
      color = Color::RED;
    }
    renderer.drawCenteredQuad(mPosition.toVec2() * (f32)config.grid.cell.size, size * 2.5f, color);

    f32 fontSize = size.x * 1.5f;
    renderer.drawText("^", (mPosition.toVec2() * (f32)config.grid.cell.size) - fontSize / 2.0f, fontSize, Color::PURPLE);
  }
  bool AndComponent::update() {
    mOutputPins[OUTPUT_INDEX].active = mInputPins[A_INPUT_INDEX].active && mInputPins[B_INPUT_INDEX].active;
    return true;
  }

  void AndComponent::renderBody(Renderer3D& renderer) {
    Vec2 size = Vec2{config.grid.cell.size3d};

    Material::Handle material = config.inactiveMaterial;
    if (mOutputPins[OUTPUT_INDEX].active) {
      material = config.activeMaterial;
    }

    Mat4 model{1.0f};
    model = glm::translate(model, mPosition.toVec3() * config.grid.cell.size3d);
    model = glm::scale(model, Vec3(Vec2(size * 2.5f), 1.0f));

    renderer.submit(config.pinMesh, material, model);
  }

}