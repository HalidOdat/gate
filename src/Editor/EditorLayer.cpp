#include <string>
#include <sstream>
#include <queue>

#include "Core/Base.hpp"
#include "Core/Input.hpp"
#include "Resource/Manager.hpp"
#include "Application.hpp"

#include "Editor/EditorLayer.hpp"

namespace Gate {

  EditorLayer::EditorLayer()
    : mEditorCameraController(Application::getWindow().getAspectRatio())
  {}

  void EditorLayer::onAttach() {
    Logger::info("EditorLayer::onAttach was called");
    Application::getWindow().setTitle(config.title);
    mGridFrameBuffer = FrameBuffer::builder()
      .clearColor(1.0f, 1.0f, 1.0f, 1.0f)
      .clear(FrameBuffer::Clear::Color | FrameBuffer::Clear::Depth)
      .clearOnBind(true)
      .attach(
        FrameBuffer::Attachment::Type::Texture,
        FrameBuffer::Attachment::Format::Rgb8
      )
      .build();
  }

  void EditorLayer::onDetach() {
    Logger::info("EditorLayer::onDetach was called");
    for (auto component : mComponents) {
      if (component) {
        delete component;
      }
    }
  }
  bool EditorLayer::isConnectionActive(Connection& connection) {
    switch (connection.type) {
      case Connection::Type::Component: {
        auto* component = mComponents[connection.componentIndex];
        auto& pins = component->getPins();
        auto& pin = pins[connection.index];
        return pin.active;
      } break;
      case Connection::Type::Wire: {
        auto& wire = mWires[connection.index];
        return wire.active;
      } break;
    }
    GATE_UNREACHABLE("");
  }
  bool EditorLayer::isConnectionVisited(Connection& connection) {
    switch (connection.type) {
      case Connection::Type::Component: {
        auto* component = mComponents[connection.componentIndex];
        auto& pins = component->getPins();
        auto& pin = pins[connection.index];
        return pin.visited;
      } break;
      case Connection::Type::Wire: {
        auto& wire = mWires[connection.index];
        return wire.visited;
      } break;
    }
    GATE_UNREACHABLE("");
  }
  ConnectionResult EditorLayer::push_wire_connection(Point position, u32 wireIndex) {
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
  ConnectionResult EditorLayer::push_component_connection(Point position, u32 componentIndex, u32 pinIndex) {
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

  bool EditorLayer::push_component(Component* component) {
    // TODO: check if it can be placed there.
    // TODO: Check if a free slot is available.
    u32 componentIndex = (u32)mComponents.size();
    
    auto& pins = component->getPins();
    for (u32 i = 0; i < pins.size(); ++i) {
      auto& pin = pins[i];      
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
  bool EditorLayer::push_wire(Wire wire) {
    // TODO: check if it can be placed there
    // TODO: Check if a free slot is available
    u32 wireIndex = (u32)mWires.size();
    auto connectionResultFrom = push_wire_connection(wire.from, wireIndex);
    if (!connectionResultFrom.successful) {
      return false;
    }
    auto connectionResultTo = push_wire_connection(wire.to, wireIndex);
    if (!connectionResultTo.successful) {
      return false;
    }
    wire.connectionIndexes.push_back(connectionResultFrom.connectionIndex);
    wire.connectionIndexes.push_back(connectionResultTo.connectionIndex);
    mWires.push_back(wire);

    tick();
    return true;
  }

  void EditorLayer::tick() {
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
          auto& pins = component->getPins();
          for (auto& pin : pins) {
            if (pin.type != Pin::Type::Input) {
              continue;
            }
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
              if (!visitedOnce && !isConnectionVisited(result)) {
                node.visitedOnce = true;
                component->setVisited(false);
                queue.push(node);
                continue;
              }
              pin.visited = isConnectionVisited(result);
              pin.active = isConnectionActive(result);
            }
          }

          if (component->areAllInputPinsVisited()) {
            component->update();
            for (auto& pin : pins) {
              if (pin.type != Pin::Type::Output) {
                continue;
              }
              
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

  void EditorLayer::renderAll(Renderer& renderer) {
    renderGrid(renderer);
    renderComponentBodys(renderer);
    renderWires(renderer);
    renderComponentConnectors(renderer);
  }

  void EditorLayer::renderComponentBodys(Renderer& renderer) {
    for (auto component : mComponents) {
      if (component) {
        component->renderBody(renderer);
      }
    }
  }

  void EditorLayer::renderComponentConnectors(Renderer& renderer) {
    for (auto component : mComponents) {
      if (component) {
        component->renderConnectors(renderer);
      }
    }
  }

  void EditorLayer::renderWires(Renderer& renderer) {
    for (auto wire : mWires) {
      wire.render(renderer);
    }
  }

  void EditorLayer::renderGrid(Renderer& renderer) {
    auto width  = Application::getWindow().getWidth();
    auto height = Application::getWindow().getHeight();

    if (!mGridTexture) {
      renderer.end();
      mGridFrameBuffer->bind();
      for (u32 i = 0; i < width; i += config.grid.cell.size) {
        for (u32 j = 0; j < height; j += config.grid.cell.size) {
          renderer.drawCenteredQuad({i, j}, {2.0f, 2.0f}, config.grid.color);
        }
      }
      renderer.flush();
      mGridFrameBuffer->unbind();  

      mGridTexture = mGridFrameBuffer->getColorAttachment(0);
    }

    renderer.clearScreen(mGridTexture);
  }

  void EditorLayer::onUpdate(Timestep ts) {
    std::stringstream ss;
    ss.precision(2);
    ss << config.title << " - " << std::fixed << (1.0f / ts) << "fps / " << ts * 1000.0f << "ms";
    std::string fpsString = ss.str();
    Application::getWindow().setTitle(fpsString.c_str());

    Application::getRenderer().begin(mEditorCameraController.getCamera());

    Application::getRenderer().clearScreen();
    renderAll(Application::getRenderer());

    auto height = Application::getWindow().getHeight();

    // Wire Draw
    if (mMode == Mode::WireDraw) {
      Wire{
        Point(mWireStartPosition / (f32)config.grid.cell.size),
        Point(mWireEndPosition   / (f32)config.grid.cell.size)
      }.render(Application::getRenderer());

      Application::getRenderer().drawCenteredQuad(mWireStartPosition, config.selector.size, config.selector.color);
      Application::getRenderer().drawCenteredQuad(mWireEndPosition, config.selector.size, config.selector.color);
    }

    // TODO: Move to UI
    switch (mMode) {
      case Mode::Select: {
        // Selector cursor
        Application::getRenderer().drawCenteredQuad(mSelectorPosition, config.selector.size, config.selector.color);
        const StringView text = " Click on the board to draw a line,\n or press \"c\" to insert a component!";
        const auto size = 16;
        Application::getRenderer().drawText(text, Vec2{size, height - 3.0f * size}, size, Color::BLACK);
      }  break;
      case Mode::WireDraw: {
        const StringView text = " Press <ESCAPE> to cancel wire drawing";
        const auto size = 16;
        Application::getRenderer().drawText(text, Vec2{size, height - 2 * size}, size, Color::BLACK);
      }  break;
      case Mode::AddComponent: {
        Application::getRenderer().drawCenteredQuad(mSelectorPosition, config.selector.size, Color::ORANGE);

        String text = " Click on board to add ";
        text += componentTypeToString(mComponentType);
        text += " component";
        const auto size = 16;
        Application::getRenderer().drawText(text, Vec2{size, height - 2 * size}, size, Color::BLACK);
      }  break;
    }
  }

  bool EditorLayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    if (event.getKey() == Key::Escape) {
      switch (mMode) {
        case Mode::Select:
          // TODO: deselect
          break;
        case Mode::WireDraw: {
          mMode = Mode::Select;
        } break;
        case Mode::AddComponent: {
          mMode = Mode::Select;
        }  break;
      }
    } else {
      switch (mMode) {
        case Mode::Select:
          if (event.getKey() == Key::C) {
            mMode = Mode::AddComponent;
          }
          break;
        case Mode::WireDraw: {
        } break;
        case Mode::AddComponent: {
          if (event.getKey() == Key::S) {
            mComponentType = ComponentType::Switch;
          } else if (event.getKey() == Key::N) {
            mComponentType = ComponentType::Not;
          }  else if (event.getKey() == Key::A) {
            mComponentType = ComponentType::And;
          }  else if (event.getKey() == Key::O) {
            mComponentType = ComponentType::Or;
          }
        }  break;
      }
    }

    #ifndef GATE_PLATFORM_WEB
      if (event.getKey() == Key::Q) {
        Application::get().quit();
      }
    #endif
    return false;
  }

  bool EditorLayer::onMouseButtonPressedEvent(const MouseButtonPressedEvent& event) {
    if (event.getButton() == MouseButton::Left) {
      mClicked = true;

      switch (mMode) {
        case Mode::Select: {
          // TODO: Check for collision
          Point mousePosition = Point(gridAlginPosition(mLastMousePosition) / (f32)config.grid.cell.size);
          bool interacted = false;
          for (auto* component : mComponents) {
            if (component->getPosition() == mousePosition) {
              component->click();
              interacted = true;
              tick();
              break;
            }
          }

          if (!interacted) {
            mMode = Mode::WireDraw;
            mWireStartPosition = gridAlginPosition(mLastMousePosition);
          }
        }  break;
        case Mode::WireDraw: {
          // TODO: Check if wires intersect
          Point from = Point(mWireStartPosition / (f32)config.grid.cell.size);
          Point to = Point(mWireEndPosition / (f32)config.grid.cell.size);
          bool connected = false;
          if (auto it = mConnectionsIndexByPoint.find(to); it != mConnectionsIndexByPoint.end()) {
            connected = true;
          }
          if (!push_wire({ from, to })) {
            GATE_TODO("implement this: wire is invalid");
          }

          // We continue wire draw
          mMode = Mode::WireDraw;
          mWireStartPosition = mWireEndPosition;
          if (connected) {
            mMode = Mode::Select;
          }
        } break;
        case Mode::AddComponent: {
          // TODO: check if it intersets withs something
          auto position = Point(getGridAlignedMousePosition() / (f32)config.grid.cell.size);
          switch (mComponentType) {
            case ComponentType::Switch: {
              push_component(new SwitchComponent(position));
            } break;
            case ComponentType::Not: {
              push_component(new NotComponent(position));
            } break;
            case ComponentType::And: {
              push_component(new AndComponent(position));
            } break;
            case ComponentType::Or: {
              push_component(new OrComponent(position));
            } break;
          }
        } break;
      }
    }
    return false;
  }
  bool EditorLayer::onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event) {
    if (event.getButton() == MouseButton::Left) {
      mClicked = false;
    }
    return false;
  }
  bool EditorLayer::onMouseMoveEvent(const MouseMoveEvent& event) {
    Vec2 gridPosition = gridAlginPosition(event.toVec2());

    switch (mMode) {
      case Mode::Select:
        mSelectorPosition = gridPosition;
        break;
      case Mode::WireDraw: {
        mWireEndPosition = gridPosition;

        Vec2 temp = glm::abs(mWireEndPosition - mWireStartPosition);
        if (temp.x > temp.y) {
          mWireEndPosition = Vec2{mWireEndPosition.x, mWireStartPosition.y};
        } else {
          mWireEndPosition = Vec2{mWireStartPosition.x, mWireEndPosition.y};
        }
      }  break;
      case Mode::AddComponent:
        mSelectorPosition = gridPosition;
        break;
    }
    mLastMousePosition = event.toVec2();
    return false;
  }

  Vec2 EditorLayer::gridAlginPosition(Vec2 position) {
    Vec2 offsetPosition = position + (config.grid.cell.size / 2.0f);
    return Vec2{
      u32(offsetPosition.x) - u32(offsetPosition.x) % config.grid.cell.size,
      u32(offsetPosition.y) - u32(offsetPosition.y) % config.grid.cell.size
    };
  }
  Vec2 EditorLayer::getGridAlignedMousePosition() {
    return gridAlginPosition(mLastMousePosition);
  }


  void EditorLayer::onUiRender(Ui& ui) {
    (void) ui;
    // ui.beginDock(Ui::Dock::Left, 20.0f);
    // ui.end();
  }

  void EditorLayer::onEvent(const Event& event) {
    event.dispatch(&EditorLayer::onWindowResizeEvent, this);
    event.dispatch(&EditorLayer::onKeyPressedEvent, this);
    event.dispatch(&EditorLayer::onMouseScrollEvent, this);
    event.dispatch(&EditorLayer::onMouseMoveEvent, this);
    event.dispatch(&EditorLayer::onMouseButtonPressedEvent, this);
    event.dispatch(&EditorLayer::onMouseButtonReleasedEvent, this);
  }

  bool EditorLayer::onWindowResizeEvent(const WindowResizeEvent& event) {
    auto[width, height] = event.getSize();
    mEditorCameraController.resize(width, height);
    mGridFrameBuffer->invalidate(width, height);
    mGridTexture = Texture::Handle();
    return false;
  }

  bool EditorLayer::onMouseScrollEvent(const MouseScrollEvent& event) {
    (void)event;
    return false;
  }

} // namespace Gate
