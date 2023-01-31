#include <string>
#include <sstream>

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

    auto* component = new SwitchComponent({2, 2});
    component->toggle();
    push_component(component);
    // push_component(new JointComponent({2, 8}));
  }

  void EditorLayer::onDetach() {
    Logger::info("EditorLayer::onDetach was called");
    for (auto component : mComponents) {
      if (component) {
        delete component;
      }
    }
  }

  bool EditorLayer::push_component(Component* component) {
    // TODO: check if it can be placed there.
    // TODO: Check if a free slot is available.
    u32 componentIndex = (u32)mComponents.size();
    
    mComponents.push_back(component);
    const auto& pins = component->getPins();

    for (u32 i = 0; i < pins.size(); ++i) {
      const auto& pin = pins[i];
      Connection::Type type;
      switch (pin.type) {
        case Pin::Type::Input:
          type = Connection::Type::ComponentIn;
          break;
        case Pin::Type::Output:
          type = Connection::Type::ComponentOut;
          break;
      }
      mConnectionsByPoint[pin.position].push_back(mConnections.size());
      mConnections.push_back(
        Connection{
          type,
          componentIndex,
          i,
        }
      );
    }
    return true;
  }
  bool EditorLayer::push_wire(Wire wire) {
    // TODO: check if it can be placed there
    // TODO: Check if a free slot is available
    u32 wireIndex = (u32)mWires.size();
    mWires.push_back(wire);

    mConnectionsByPoint[wire.from].push_back(mConnections.size());
    mConnections.push_back(
      Connection{
        Connection::Type::WireFrom,
        UINT32_MAX,
        wireIndex,
      }
    );
    
    mConnectionsByPoint[wire.to].push_back(mConnections.size());
    mConnections.push_back(
      Connection{
        Connection::Type::WireTo,
        UINT32_MAX,
        wireIndex,
      }
    );
    return true;
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

    tick();

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
      case Mode::Select:
        // Selector cursor
        Application::getRenderer().drawCenteredQuad(mSelectorPosition, config.selector.size, config.selector.color);
        break;
      case Mode::WireDraw:
        const StringView text = " Press <ESCAPE> to cancel wire drawing";
        const auto size = 16;
        Application::getRenderer().drawText(text, Vec2{size, height - 2 * size}, size, Color::BLACK);
        break;
    }
  }

  void EditorLayer::tickConnections(std::vector<u32>& connectionIndexes, bool active) {
    for (const auto& connectionIndex : connectionIndexes) {
      auto& connection = mConnections[connectionIndex];
      switch (connection.type) {
      case Connection::Type::ComponentIn: {
        GATE_UNREACHABLE("");
      } break;
      case Connection::Type::ComponentOut: {
        // TODO: Do we need this?
        // GATE_UNREACHABLE("");
      } break;
      case Connection::Type::WireFrom: {
        auto& wire = mWires[connection.index];
        wire.active = active;
        // if (auto it = mConnectionsByPoint.find(wire.from); it != mConnectionsByPoint.end()) {
        //   auto[point, connectionIndexes] = *it;
        //   tickConnections(connectionIndexes, active);
        // }
      } break;
      case Connection::Type::WireTo: {
        auto& wire = mWires[connection.index];
        wire.active = active;
        // if (auto it = mConnectionsByPoint.find(wire.to); it != mConnectionsByPoint.end()) {
        //   auto[point, connectionIndexes] = *it;
        //   tickConnections(connectionIndexes, active);
        // }
      } break;
      }
    }
  }

  void EditorLayer::tick() {
    for (auto component : mComponents) {
      //   auto& ins = component->getInputPins();
      //   for (auto& in : ins) {
      //     Logger::error("TODO: fix in pins");
      //   }

      component->update();
      auto& pins = component->getPins();
      for (auto& pin : pins) {
        if (auto it = mConnectionsByPoint.find(pin.position); it != mConnectionsByPoint.end()) {
          auto[point, connectionIndexes] = *it;
          tickConnections(connectionIndexes, pin.active);
        }
      }
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
        }
      }
    }
    
    if (event.getKey() == Key::A) {
      for (auto& component : mComponents) {
        component->click();
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
          push_wire({ from, to });

          // We continue wire draw
          mMode = Mode::WireDraw;
          mWireStartPosition = mWireEndPosition;
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
      case Mode::WireDraw:
        mWireEndPosition = gridPosition;

        Vec2 temp = glm::abs(mWireEndPosition - mWireStartPosition);
        if (temp.x > temp.y) {
          mWireEndPosition = Vec2{mWireEndPosition.x, mWireStartPosition.y};
        } else {
          mWireEndPosition = Vec2{mWireStartPosition.x, mWireEndPosition.y};
        }
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
