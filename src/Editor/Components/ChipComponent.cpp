#include "Application.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components/Component.hpp"

#include "Editor/Chip.hpp"

namespace Gate {

  ChipComponent::ChipComponent(Point position, Chip::Handle chip)
    : Component(Component::Category::Gate, Type::Chip, position), mChip{chip}
  {
    auto[inputs, outputs] = mChip->getPinComponents();

    this->mChipInputs = inputs;
    this->mChipOutputs = outputs;

    for (u32 i = 0; i < mChipInputs.size(); ++i) {
      this->mInputPins.push_back(Pin{Point{position.x - 1, position.y + i}});
    }

    for (u32 i = 0; i < mChipOutputs.size(); ++i) {
      this->mOutputPins.push_back(Pin{Point{position.x + 1, position.y + i}});
    }
  }
  void ChipComponent::renderBody(Renderer2D& renderer) {
    Vec2 size = Vec2{(f32)config.grid.cell.size};
    Vec4 color = Color::BLACK;
    // if (mOutputPins[OUTPUT_INDEX].active) {
    //   color = Color::RED;
    // }
    auto width = std::max(this->mChipInputs.size(), this->mChipOutputs.size());
    renderer.drawCenteredQuad(mPosition.toVec2() * (f32)config.grid.cell.size - Vec2{1.0f, f32(width)} / 2.0f, size * 1.8f * Vec2{1.0f, f32(width)}, color);
  }
  bool ChipComponent::update() {
    for (usize i = 0; i < mChipInputs.size(); ++i) {
      mChipInputs[i]->getOutputPins()[SwitchComponent::OUTPUT_INDEX].active = mInputPins[i].active;
    }

    mChip->tick();

    for (usize i = 0; i < mChipOutputs.size(); ++i) {
      mOutputPins[i].active = mChipOutputs[i]->getInputPins()[OutputComponent::INPUT_INDEX].active;
    }

    return true;
  }

  void ChipComponent::renderBody(Renderer3D& renderer, u32 id) {
    Material::Handle material = config.inactiveMaterial;
    // if (mOutputPins[OUTPUT_INDEX].active) {
    //   material = config.activeMaterial;
    // }

    auto width = std::max(this->mChipInputs.size(), this->mChipOutputs.size());
    
    f32 size = 1.5f;

    Vec3 position = mPosition.toVec3() * config.grid.cell.size3d * Vec3{1.0f, -1.0f, 0.0f} + Vec3{0.0f, 0.0f, config._3dZOffset};
    Mat4 model = glm::translate(Mat4{1.0f}, position);

    size = config.grid.cell.size3d * size;
    model = glm::scale(model, Vec3(size, size * width, size));

    renderer.submit(config.pinMesh, material, model, id);
  }

  Serializer::Node ChipComponent::encode() const { 
    using namespace Serializer;

    auto node = Node::object();
    node["type"]     = String("ChipComponent");
    node["position"] = Convert<Point>::encode(mPosition);
    node["chip"]     = mChip->getIndex();
    return node;
  }

}