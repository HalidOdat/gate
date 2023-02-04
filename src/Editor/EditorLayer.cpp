#include "Core/Base.hpp"
#include "Core/Input.hpp"
#include "Application.hpp"

#include "Editor/EditorLayer.hpp"

#include <queue>

namespace Gate {

  EditorLayer::EditorLayer()
    : mEditorCameraController(Application::getWindow().getAspectRatio())
  {
    Logger::trace("EditorLayer: Constructor was called");
    const auto themeFilepath = "assets/themes/default.json";
    auto theme = Theme::Settings::load(themeFilepath);
    if (theme) {
      config.apply(*theme);
    } else {
      Logger::error("Unable to apply theme %s", themeFilepath);
    }
  }
  void EditorLayer::onUpdate(Timestep ts) {
    (void)ts;
    Application::getRenderer().begin(mEditorCameraController.getCamera());

    mBoard.render(Application::getRenderer());

    // Application::getRenderer().drawCenteredCircle(mSelectorPosition, 100, Color::RED, 0.2f, 1.01f);
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
          } else if (event.getKey() == Key::A) {
            mComponentType = ComponentType::And;
          } else if (event.getKey() == Key::O) {
            mComponentType = ComponentType::Or;
          } else if (event.getKey() == Key::X) {
            mComponentType = ComponentType::Xor;
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
          bool interacted = mBoard.getCurrentChip().click(mousePosition);

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
          switch (mBoard.getCurrentChip().push_wire({ from, to })) {
            case WirePushState::Valid:
              break;
            case WirePushState::Invalid:
              GATE_TODO("implement this: wire is invalid");
              break;
            case WirePushState::Connected:
              connected = true;
              break;
          }

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
              mBoard.getCurrentChip().push_component(new SwitchComponent(position));
            } break;
            case ComponentType::Not: {
              mBoard.getCurrentChip().push_component(new NotComponent(position));
            } break;
            case ComponentType::And: {
              mBoard.getCurrentChip().push_component(new AndComponent(position));
            } break;
            case ComponentType::Or: {
              mBoard.getCurrentChip().push_component(new OrComponent(position));
            } break;
            case ComponentType::Xor: {
              mBoard.getCurrentChip().push_component(new XorComponent(position));
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
    mBoard.onResize(width, height);
    return false;
  }

  bool EditorLayer::onMouseScrollEvent(const MouseScrollEvent& event) {
    (void)event;
    return false;
  }

} // namespace Gate
