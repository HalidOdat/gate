#include "Editor/Components/Component.hpp"
#include "Editor/Config.hpp"
#include "Editor/Components.hpp"

#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Gate {

  Component::~Component() {}

  void Component::renderConnectors(Renderer2D& renderer) {
    // TODO: Show when outputs and inputs are active
    for (auto& pin : mInputPins) {
      pin.render(renderer, false);
    }
    for (auto& pin : mOutputPins) {
      pin.render(renderer, true);
    }
  }
  void Component::resetVisited() {
    setVisited(false);
    for (auto& pin : mInputPins) {
      pin.visited = false;
    }
    for (auto& pin : mOutputPins) {
      pin.visited = false;
    }
  }
  bool Component::areAllInputPinsVisited() const {
    for (auto& pin : mInputPins) {
      if (!pin.visited) {
        return false;
      }
    }
    return true;
  }

  void Component::renderConnectors(Renderer3D& renderer, u32 id) {
    for (auto& pin : mInputPins) {
      pin.render(renderer, false, id);
    }
    for (auto& pin : mOutputPins) {
      pin.render(renderer, true, id);
    }
  }

  Component* Component::decode(const Serializer::Node& node) {
    using namespace Serializer;
    if (!node.isObject()) return nullptr;

    Point position;
    if (auto* positionNode = node.get("position"); positionNode) {
      if (!Convert<Point>::decode(*positionNode, position)) return nullptr;
    } else {
      return nullptr;
    }

    if (auto* typeNode = node.get("type"); typeNode) {
      if (!typeNode->isString()) return nullptr;

      auto& type = *typeNode->asString();
      if (type == "SwitchComponent") {
        return new SwitchComponent(position);
      } else if (type == "AndComponent") {
        return new AndComponent(position);
      } else if (type == "OrComponent") {
        return new OrComponent(position);
      } else if (type == "XorComponent") {
        return new XorComponent(position);
      } else if (type == "NotComponent") {
        return new NotComponent(position);
      } else {
        Logger::error("Json: invalid component type: %s", type.c_str());
        return nullptr;
      }
    }
    return nullptr;
  }

}