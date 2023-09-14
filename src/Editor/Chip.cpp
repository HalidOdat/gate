#include "Editor/Chip.hpp"
#include "Application.hpp"

#include <queue>

namespace Gate {
  
  Chip::Handle Chip::create(u32 index) {
    return std::make_shared<Chip>(index);
  }

  Chip::Chip(u32 index)
    : mName{String("chip ") + std::to_string(index)}
  {
    mOptimalCellSize = config.grid.cell.size;
    mIndex = index;
  }
  Chip::~Chip() {
    for (auto component : mComponents) {
      if (component) {
        delete component;
      }
    }
  }

  bool Chip::click(Point position) {
    bool interacted = false;
    for (auto* component : mComponents) {
      if (!component) {
        continue;
      }
      if (component->getPosition() == position) {
        component->click();
        interacted = true;
        tick();
        break;
      }
    }
    return interacted;
  }

  bool Chip::click(u32 id) {
    bool interacted = false;
    if (id < mComponents.size()) {
      mComponents[id]->click();
      interacted = true;
      tick();
    }
    return interacted;
  }
  void Chip::calculateOptimalCellSize(Point position) {
    auto wWidth  = Application::getWindow().getWidth();
    auto wHeight = Application::getWindow().getHeight();

    const auto padding = 1;

    if (position.x * mOptimalCellSize >= wWidth) {
      mOptimalCellSize = wWidth / position.x - padding;
    }
    if (position.y * mOptimalCellSize >= wHeight) {
      mOptimalCellSize = wHeight / position.y - padding;
    }

    if (mOptimalCellSize < 2) {
      mOptimalCellSize = 2;
    }
  }
  ConnectionState Chip::getConnectionState(Connection& connection) {
    switch (connection.type) {
      case Connection::Type::Component: {
        auto* component = mComponents[connection.componentIndex];
        auto& pins = component->getOutputPins();
        auto& pin = pins.at(connection.index);
        return ConnectionState(pin.active, pin.visited);
      } break;
      case Connection::Type::Wire: {
        auto& wire = mWires[connection.index];
        if (wire.free) {
          GATE_UNREACHABLE("We shouldn't access freed wires!");
        }
        return ConnectionState(wire.active, wire.visited);
      } break;
    }
    GATE_UNREACHABLE("");
  }
  ConnectionResult Chip::pushWireConnection(Point position, u32 wireIndex) {
    auto connection = Connection {
      Connection::Type::Wire,
      wireIndex,
    };
    u32 connectionIndex;
    if (auto it = mConnectionsIndexByPoint.find(position); it != mConnectionsIndexByPoint.end()) {
      connectionIndex = it->second;
      mConnections[connectionIndex].push_back(connection);
    } else {
      connectionIndex = (u32)mConnections.size();
      mConnections.push_back({});
      mConnections[connectionIndex].push_back(connection);
      mConnectionsIndexByPoint[position] = connectionIndex;
    }
    return connectionIndex;
  }
  ConnectionResult Chip::pushComponentConnection(Point position, u32 componentIndex, u32 pinIndex) {
    auto connection = Connection {
      Connection::Type::Component,
      pinIndex,
      componentIndex
    };
    u32 connectionIndex;
    if (auto it = mConnectionsIndexByPoint.find(position); it != mConnectionsIndexByPoint.end()) {
      connectionIndex = it->second;
      // TODO: check if its output pin
      if (false) {
        return {};
      }
      mConnections[connectionIndex].push_back(connection);
    } else {
      connectionIndex = (u32)mConnections.size();
      mConnections.push_back({});
      mConnections[connectionIndex].push_back(connection);
      mConnectionsIndexByPoint[position] = connectionIndex;
    }
    return connectionIndex;
  }

  bool Chip::pushComponent(Component* component) {
    for (auto* oldComponent : mComponents) {
      if (oldComponent && oldComponent->getPosition() == component->getPosition()) {
        // delete component;
        return false;
      }
    }
    
    u32 freeSlot = 0;
    for (; freeSlot < mComponents.size(); ++freeSlot) {
      auto* component = mComponents[freeSlot];
      if (!component) {
        break;
      }
    }
    // There are no free slots
    if (freeSlot == mComponents.size()) {
      mComponents.push_back(nullptr);
    }
    u32 componentIndex = freeSlot;
    
    // TODO: refactor to remove code duplication
    auto& inputPins = component->getInputPins();
    for (u32 i = 0; i < inputPins.size(); ++i) {
      auto& pin = inputPins[i];      
      auto[successful, connectionIndex] = pushComponentConnection(pin.position, componentIndex, i);
      if (!successful) {
        GATE_TODO("Fix invalid pin connection!");
      }
      mConnectionsIndexByPoint[pin.position] = connectionIndex;
      pin.connectionIndex = connectionIndex;
    }
    auto& outputPins = component->getOutputPins();
    for (u32 i = 0; i < outputPins.size(); ++i) {
      auto& pin = outputPins[i];      
      auto[successful, connectionIndex] = pushComponentConnection(pin.position, componentIndex, i);
      if (!successful) {
        GATE_TODO("Fix invalid pin connection!");
      }
      mConnectionsIndexByPoint[pin.position] = connectionIndex;
      pin.connectionIndex = connectionIndex;
    }
    mComponents[componentIndex] = component;
    tick();

    calculateOptimalCellSize(component->getPosition());
    return true;
  }
  void Chip::removeComponent(Point position) {
    for (u32 i = 0; i < mComponents.size(); ++i) {
      auto* component = mComponents[i];
      if (!component) {
        continue;
      }
      if (component->getPosition() == position) {
        if (!component->deletable()) {
          return;
        }
        for (auto& pin : component->getOutputPins()) {
          std::vector<Connection>& connections = mConnections[pin.connectionIndex];
          std::vector<Connection> result;
          for (auto& connection : connections) {
            bool isComponentPin = connection.type == Connection::Type::Component && connection.componentIndex == i;
            if (!isComponentPin) {
              result.push_back(connection);
            }
          }
          mConnections[pin.connectionIndex] = result;
        }
        for (auto& pin : component->getInputPins()) {
          std::vector<Connection>& connections = mConnections[pin.connectionIndex];
          std::vector<Connection> result;
          for (auto& connection : connections) {
            bool isComponentPin = connection.type == Connection::Type::Component && connection.componentIndex == i;
            if (!isComponentPin) {
              result.push_back(connection);
            }
          }
          mConnections[pin.connectionIndex] = result;
        }
        delete component;
        mComponents[i] = nullptr;
        tick();
        break;
      }
    }
  }
  WirePushState Chip::pushWire(Wire wire) {
    // TODO: Check if wire intersects with another wire
    // Wire of length 0
    if (wire.from == wire.to) {
      return WirePushState::Connected;
    }

    bool connected = false;
    if (auto it = mConnectionsIndexByPoint.find(wire.to); it != mConnectionsIndexByPoint.end()) {
      connected = true;
    }

    // Swap if needed
    if (wire.from.x > wire.to.x) {
      auto temp = wire.to.x;
      wire.to.x = wire.from.x;
      wire.from.x = temp;
    }
    if (wire.from.y > wire.to.y) {
      auto temp = wire.to.y;
      wire.to.y = wire.from.y;
      wire.from.y = temp;
    }

    if (wire.from.x != wire.to.x && wire.from.y != wire.to.y) {
      GATE_UNREACHABLE("lines should be horizontal or vertical!");
    }

    u32 freeSlot = 0;
    for (; freeSlot < mWires.size(); ++freeSlot) {
      if (mWires[freeSlot].free) {
        break;
      }
    }
    if (freeSlot >= mWires.size()) {
      mWires.push_back(wire);
    } else {
      mWires[freeSlot] = wire;
    }

    mWires[freeSlot].free = true;

    auto connectionResultFrom = pushWireConnection(mWires[freeSlot].from, freeSlot);
    if (!connectionResultFrom.successful) {
      return WirePushState::Invalid;
    }
    auto connectionResultTo = pushWireConnection(mWires[freeSlot].to, freeSlot);
    if (!connectionResultTo.successful) {
      return WirePushState::Invalid;
    }
    mWires[freeSlot].connectionIndexes.push_back(connectionResultFrom.connectionIndex);
    mWires[freeSlot].connectionIndexes.push_back(connectionResultTo.connectionIndex);

    mWires[freeSlot].free = false;

    tick();
    calculateOptimalCellSize(mWires[freeSlot].from);
    calculateOptimalCellSize(mWires[freeSlot].to);
    return connected ? WirePushState::Connected : WirePushState::Valid;
  }
  void Chip::removeWire(Point position) {
    bool hasRemoved = false;
    for (u32 i = 0; i < mWires.size(); ++i) {
      auto& wire = mWires[i];
      if (wire.free) {
        continue;
      }
      
      if (wire.from.y == wire.to.y) {        // Horizontal line
        if (position.y == wire.to.y && position.x >= wire.from.x && position.x <= wire.to.x) {
          wire.free = true;
          hasRemoved = true;
          Logger::trace("Horizontal Wire removed");
        }
      } else if (wire.from.x == wire.to.x) { // Vertical line
        if (position.x == wire.to.x && position.y >= wire.from.y && position.y <= wire.to.y) {
          Logger::trace("Vertical Wire removed");
          wire.free = true;
          hasRemoved = true;
        }
      } else {
        GATE_UNREACHABLE("There should only be horizontal or veritcal lines!");
      }
      
      // If wire has been freed, delete connections
      if (wire.free) {
        for (auto& connectionIndex : wire.connectionIndexes) {
          auto& connections = mConnections[connectionIndex];
          std::vector<Connection> result;
          for (auto& connection : connections) {
            if (connection.type == Connection::Type::Wire && connection.index == i) {
              continue;
            }
            result.push_back(connection);
          }
          mConnections[connectionIndex] = result;
        }
      }
    }

    if (hasRemoved) {
      tick();
    }
  }

  void Chip::tick() {
    enum class Type {
      Component,
      Wire,
    };
    struct Node {
      Type type;
      u32 index;
      bool activate;
      Component* component = nullptr;

      bool visitedOnce = false;

      bool operator<(const Node &rhs) const {
        return (u32)this->visitedOnce > (u32)rhs.visitedOnce;
      }
    };

    std::priority_queue<Node, std::vector<Node>> queue;
    auto enqueueComponent = [&](u32 index, bool activate) {
      if (mComponents[index]->isVisited()) {
        return;
      }
      queue.push(
        Node {
          Type::Component,
          index,
          activate,
          mComponents[index]
        }
      );
    };
    auto enqueueWire = [&](u32 index, bool activate) {
      if (mWires[index].free) {
        GATE_UNREACHABLE("We shouldn't enque a freed wire!");
      }
      if (mWires[index].visited) {
        return;
      }
      queue.push(
        Node {
          Type::Wire,
          index,
          activate,
        }
      );
    };
    auto enqueueConnections = [&](std::vector<Connection>& connections, bool activate) {
      for (auto& connection : connections) {
        switch (connection.type) {
          case Connection::Type::Component: {
            enqueueComponent(connection.componentIndex, activate);
          } break;
          case Connection::Type::Wire: {
            enqueueWire(connection.index, activate);
          } break;
        }
      }
    };

    // Clear visited flags on components and wires
    for (usize i = 0; i < mComponents.size(); ++i) {
      auto* component = mComponents[i];
      if (!component) {
        continue;
      }
      component->resetVisited();
      if (component->getCategory() == Component::Category::Input) {
        enqueueComponent((u32)i, true);
      }
    }
    for (auto& wire : mWires) {
      if (wire.free) {
        continue;
      }
      wire.visited = false;
      wire.active = false;
    }

    for (; !queue.empty(); queue.pop()) {
      auto node = queue.top();
      auto&[type, index, activate, component, visitedOnce] = node;
      switch (type) {
        case Type::Component: {
          component->setVisited(true);
          auto& pins = component->getInputPins();
          for (u32 pinIndex = 0; pinIndex < pins.size(); ++pinIndex) {
            auto& pin = pins[pinIndex];
            if (pin.visited) {
              continue;
            }

            auto& connections = mConnections[pin.connectionIndex];
            ConnectionState state;
            for (auto& connection : connections) {    
              // Exclude the current pin
              if (
                connection.type == Connection::Type::Component
                && mComponents[connection.componentIndex] == component
                && connection.index == pinIndex
              ) {
                continue;
              }

              auto newState = getConnectionState(connection);
              if (newState.visited) {
                state.visited = true;
                state.active  = state.active || newState.active;
              }
            }
            if (!(visitedOnce || state.visited)) {
              node.visitedOnce = true;
              component->setVisited(false);
              queue.push(node);
              continue;
            }
            pin.visited = state.visited;
            pin.active = state.active;
          }

          if (component->areAllInputPinsVisited()) {
            component->update();
            for (auto& pin : component->getOutputPins()) {
              pin.visited = true;
              auto& connections = mConnections[pin.connectionIndex];
              enqueueConnections(connections, pin.active);
            }
          }
        } break;
        case Type::Wire: {
          auto& wire = mWires[index];
          if (wire.free) {
            GATE_UNREACHABLE("We shouldn't access a freed wire!");
          }
          wire.visited = true;
          wire.active = activate;
          for (auto& connectionIndex : wire.connectionIndexes) {
            auto& connection = mConnections[connectionIndex];
            enqueueConnections(connection, wire.active);
          }
        } break;
      } 
    }
  }

  void Chip::render(Renderer2D& renderer) {
    renderComponentBodys(renderer);
    renderWires(renderer);
    renderComponentConnectors(renderer);
  }
  void Chip::renderComponentBodys(Renderer2D& renderer) {
    for (auto component : mComponents) {
      if (component) {
        component->renderBody(renderer);
      }
    }
  }
  void Chip::renderComponentConnectors(Renderer2D& renderer) {
    for (auto component : mComponents) {
      if (component) {
        component->renderConnectors(renderer);
      }
    }
  }
  void Chip::renderWires(Renderer2D& renderer) {
    for (auto wire : mWires) {
      if (wire.free) {
        continue;
      }
      wire.render(renderer);
    }
  }

  // ================================= 3D =====================================

  void Chip::render(Renderer3D& renderer) {
    renderComponentBodys(renderer);
    renderWires(renderer);
    renderComponentConnectors(renderer);
  }
  void Chip::renderComponentBodys(Renderer3D& renderer) {
    for (u32 i = 0; i < mComponents.size(); ++i) {
      auto* component = mComponents[i];
      if (component) {
        component->renderBody(renderer, i);
      }
    }
  }
  void Chip::renderComponentConnectors(Renderer3D& renderer) {
    for (u32 i = 0; i < mComponents.size(); ++i) {
      auto* component = mComponents[i];
      if (component) {
        component->renderConnectors(renderer, i);
      }
    }
  }
  void Chip::renderWires(Renderer3D& renderer) {
    for (auto wire : mWires) {
      if (wire.free) {
        continue;
      }
      wire.render(renderer);
    }
  }

  std::pair<std::vector<SwitchComponent*>, std::vector<OutputComponent*>> Chip::getPinComponents() const {
    std::vector<SwitchComponent*> inputs;
    std::vector<OutputComponent*> outputs;

    for (auto component : mComponents) {
      if (component->getType() == Component::Type::Switch) {
        inputs.push_back((SwitchComponent*)component);
      } if (component->getType() == Component::Type::Output) {
        outputs.push_back((OutputComponent*)component);
      }
    }

    return {inputs, outputs};
  }

}

namespace Gate::Serializer {

  Node Convert<Chip>::encode(Chip& chip) {
    auto node = Node::object();
    node["name"] = chip.getName();
    auto wires = Node::array();
    for (auto& wire : chip.getWires()) {
      if (wire.free) {
        continue;
      }
      wires.push(Convert<Wire>::encode(wire));
    }
    auto components = Node::array();
    for (const auto* component : chip.getComponents()) {
      if (!component) {
        continue;
      }
      components.push(component->encode());
    }
    node["wires"] = wires;
    node["components"] = components;
    return node;
  }
  bool Convert<Chip>::decode(const Node& node, Chip& chip, Board& board) {
    if (!node.isObject()) return false;
    auto* nameNode = node.get("name");
    if (!nameNode || !nameNode->isString()) return false;
    chip.setName(*nameNode->asString());

    auto* wiresNode = node.get("wires");
    if (!wiresNode || !wiresNode->isArray()) return false;
    auto& wiresArray = *wiresNode->asArray();
    for (auto& wireNode : wiresArray) {
      Wire wire;
      Convert<Wire>::decode(wireNode, wire);
      chip.pushWire(wire);
    }

    auto* componentsNode = node.get("components");
    if (!componentsNode || !componentsNode->isArray()) return false;
    auto& componentsArray = *componentsNode->asArray();
    for (auto& componentNode : componentsArray) {
      Component* component = Component::decode(componentNode, board);
      if (!component) return false;
      chip.pushComponent(component);
    }

    return true;
  }

}
