#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>


#include "Core/Log.hpp"
#include "Layers/ExampleLayer.hpp"

namespace Game {

  struct TagComponent : public Component<TagComponent> {
    TagComponent(const char* string)
      : string(string)
    {}
    const char* string;
  };

  struct PositionComponent : public Component<PositionComponent> {
    PositionComponent(const Vec3& position)
      : position(position)
    {}
    Vec3 position;
  };

  ExampleLayer::ExampleLayer() {}

  void ExampleLayer::onAttach() {
    Logger::info("ExampleLayer::onAttach was called");

    // for (auto entity : this->registry.view<TagComponent>()) {
    //   Logger::info("ECS: onAttach 1: Entity: %u", entity);
    // }

    auto entity1 = this->registry.create();
    auto entity2 = this->registry.create();
    auto entity3 = this->registry.create();

    this->registry.assign<TagComponent>(entity1, "hello - 1");
    this->registry.assign<TagComponent>(entity2, "hello - 2");
    this->registry.assign<TagComponent>(entity3, "hello - 3");

    this->registry.assign<PositionComponent>(entity3, Vec3{0.0f, 1000.0f, 0.0f});

    Logger::info("................................");

    for (auto[entity, tag, position] : this->registry.view<TagComponent, PositionComponent>()) {
      Logger::info("ECS: onAttach: Entity: %u : tag: %s : position: %f, %f, %f", entity, tag.string, position.position.x, position.position.y, position.position.z);
    }
  }

  void ExampleLayer::onDetach() {
    Logger::info("ExampleLayer::onDetach was called");
  }

  void ExampleLayer::onUpdate(Timestep ts) {

  }

  void ExampleLayer::onUiRender(Ui& ui) {
    ui.begin({0.0f, 0.0f});
      if (ui.button({1.0f, 1.0f, 0.0f})) {
        Logger::info("Button clicked!!!");
      }
    ui.end();
  }

  void ExampleLayer::onEvent(const Event& event) {
    event.dispatch(&ExampleLayer::onKeyPressedEvent, this);
  }

  bool ExampleLayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    return false;
  }

} // namespace Game