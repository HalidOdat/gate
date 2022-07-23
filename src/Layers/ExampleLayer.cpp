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

  ExampleLayer::ExampleLayer() {}

  void ExampleLayer::onAttach() {
    Logger::info("ExampleLayer::onAttach was called");

    for (auto entity : this->registry.view<TagComponent>()) {
      Logger::info("ECS: onAttach 1: Entity: %u", entity);
    }

    auto entity1 = this->registry.create();
    auto entity2 = this->registry.create();
    auto entity3 = this->registry.create();

    for (auto entity : this->registry.view<TagComponent>()) {
      Logger::info("ECS: onAttach 2: Entity: %u", entity);
    }

    this->registry.assign<TagComponent>(entity1, "hello - 1");

    for (auto entity : this->registry.view<TagComponent>()) {
      Logger::info("ECS: onAttach 3: Entity: %u", entity);
    }

    this->registry.assign<TagComponent>(entity2, "hello - 2");

    for (auto entity : this->registry.view<TagComponent>()) {
      Logger::info("ECS: onAttach 4: Entity: %u", entity);
    }

    this->registry.assign<TagComponent>(entity3, "hello - 3");
    
    for (auto entity : this->registry.view<TagComponent>()) {
      Logger::info("ECS: onAttach 5: Entity: %u", entity);
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