#include "Editor/Chip.hpp"

#include <queue>

namespace Gate {
  
  Chip::Chip(String name)
    : mName{std::move(name)}
  {}
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
      if (component->getPosition() == position) {
        component->click();
        interacted = true;
        tick();
        break;
      }
    }
    return interacted;
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
        return ConnectionState(wire.active, wire.visited);
      } break;
    }
    GATE_UNREACHABLE("");
  }
  ConnectionResult Chip::push_wire_connection(Point position, u32 wireIndex) {
    auto connection = Connection {
      Connection::Type::Wire,
      wireIndex,
    };
    u32 connectionIndex;
    if (auto it = mConnectionsIndexByPoint.find(position); it != mConnectionsIndexByPoint.end()) {
      connectionIndex = it->second;
      // TODO: check if its output pin, shouldn't have more than one connection.
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
  ConnectionResult Chip::push_component_connection(Point position, u32 componentIndex, u32 pinIndex) {
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

  bool Chip::push_component(Component* component) {
    // TODO: check if it can be placed there.
    // TODO: Check if a free slot is available.
    u32 componentIndex = (u32)mComponents.size();
    
    // TODO: refactor to remove code duplication
    auto& inputPins = component->getInputPins();
    for (u32 i = 0; i < inputPins.size(); ++i) {
      auto& pin = inputPins[i];      
      auto[successful, connectionIndex] = push_component_connection(pin.position, componentIndex, i);
      if (!successful) {
        GATE_TODO("Fix invalid pin connection!");
      }
      mConnectionsIndexByPoint[pin.position] = connectionIndex;
      pin.connectionIndex = connectionIndex;
    }
    auto& outputPins = component->getOutputPins();
    for (u32 i = 0; i < outputPins.size(); ++i) {
      auto& pin = outputPins[i];      
      auto[successful, connectionIndex] = push_component_connection(pin.position, componentIndex, i);
      if (!successful) {
        GATE_TODO("Fix invalid pin connection!");
      }
      mConnectionsIndexByPoint[pin.position] = connectionIndex;
      pin.connectionIndex = connectionIndex;
    }
    mComponents.push_back(component);
    tick();
    return true;
  }
  WirePushState Chip::push_wire(Wire wire) {
    // TODO: check if it can be placed there
    // TODO: Check if a free slot is available
    bool connected = false;
    if (auto it = mConnectionsIndexByPoint.find(wire.to); it != mConnectionsIndexByPoint.end()) {
      connected = true;
    }

    u32 wireIndex = (u32)mWires.size();
    auto connectionResultFrom = push_wire_connection(wire.from, wireIndex);
    if (!connectionResultFrom.successful) {
      return WirePushState::Invalid;
    }
    auto connectionResultTo = push_wire_connection(wire.to, wireIndex);
    if (!connectionResultTo.successful) {
      return WirePushState::Invalid;
    }
    wire.connectionIndexes.push_back(connectionResultFrom.connectionIndex);
    wire.connectionIndexes.push_back(connectionResultTo.connectionIndex);
    mWires.push_back(wire);

    tick();
    return connected ? WirePushState::Connected : WirePushState::Valid;
  }

  /// TODO: Refactor and make this a bit better
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
      component->resetVisited();
      if (component->getCategory() == Component::Category::Input) {
        enqueueComponent((u32)i, true);
      }
    }
    for (auto& wire : mWires) {
      wire.visited = false;
      wire.active = false;
    }

    for (; !queue.empty(); queue.pop()) {
      auto node = queue.top();
      auto&[type, index, activate, component, visitedOnce] = node;
      switch (type) {
        case Type::Component: {
          component->setVisited(true);
          for (auto& pin : component->getInputPins()) {
            if (pin.visited) {
              continue;
            }

            auto& connections = mConnections[pin.connectionIndex];
            if (connections.size() > 2) {
              Logger::error("too many connection on imput type point(%d, %d)", pin.position.x, pin.position.y);
              GATE_TODO("^");
            }
            if (connections.size() == 2) {
              Connection result{Connection::Type::Component, UINT32_MAX, UINT32_MAX};
              for (auto& connection : connections) {    
                if (
                  connection.type == Connection::Type::Component
                  && mComponents[connection.componentIndex] == component
                ) {
                  continue;
                }
                result = connection;
                
              }
              auto state = getConnectionState(result);
              if (!(visitedOnce || state.visited)) {
                node.visitedOnce = true;
                component->setVisited(false);
                queue.push(node);
                continue;
              }
              pin.visited = state.visited;
              pin.active = state.active;
            }
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

    const auto size = 16;
    String text = "Name: " + mName;
    renderer.drawText(text, Vec2{size}, (f32)size, config.text.color);
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
      wire.render(renderer);
    }
  }

}