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

    mComponents.push_back(new Component{"Hello"});
  }

  void EditorLayer::onDetach() {
    Logger::info("EditorLayer::onDetach was called");
    for (auto component : mComponents) {
      if (component) {
        delete component;
      }
    }
  }

  void EditorLayer::renderAll(Renderer& renderer) {
    renderGrid(renderer);
    renderComponents(renderer);
    renderWires(renderer);
  }

  void EditorLayer::renderComponents(Renderer& renderer) {
    for (auto component : mComponents) {
      if (component) {
        component->render(renderer);
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
      Application::getRenderer().drawQuad(mWireStartPosition - Vec2{config.wire.width}/2.0f, mWireSize, config.wire.inactiveColor);
    }

    // Selector
    Application::getRenderer().drawCenteredQuad(mSelectorPosition, config.selector.size, config.selector.color);

    // TODO: Move to UI
    switch (mMode) {
      case Mode::Select:
        break;
      case Mode::WireDraw:
        const StringView text = "Press <ESCAPE> to cancel wire drawing";
        const auto size = 20;
        const auto x = size * (text.size()/2);
        Application::getRenderer().drawText(text, {x, height - size*2}, 20, Color::BLACK);
        break;
    }
  }

  bool EditorLayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    if (event.getKey() == Key::Escape) {
      switch (mMode) {
        case Mode::Select:
          // TODO: deselect
          break;
        case Mode::WireDraw:
          // TODO: Check if wires intersect
          // TODO: assert from is smaller than to
          mWires.push_back(Wire{Point(mWireStartPosition), Point(mWireEndPosition)});
          mMode = Mode::Select;
          break;
      }
    }
    
    if (event.getKey() == Key::A) {
      for (auto& wire : mWires) {
        wire.active = !wire.active;
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

      // u32 entityId = Renderer::readPixel(mLastMousePosition.x, mLastMousePosition.y);
      // if (entityId != UINT32_MAX) {
      //   Logger::trace("Editor: Selected entity id: 0x%x", entityId);
      //   // mSelectedEntity = {entityId, mEditorScene.get()};
      //   // Renderer::setSelectedEntity(entityId);
      // }

      mWireStartPosition = gridAlginPosition(mLastMousePosition);

      switch (mMode) {
        case Mode::Select:
          // TODO: Check for collision
          mMode = Mode::WireDraw;
          break;
        case Mode::WireDraw:
          // TODO: Save wire, if valid
          break;
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

        Vec2 temp = mWireStartPosition - mWireEndPosition;
        if (temp.x < temp.y) {
          mWireSize = {mWireEndPosition.x - mWireStartPosition.x, config.wire.width};
        } else {
          mWireSize = {config.wire.width, mWireEndPosition.y - mWireStartPosition.y};
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
