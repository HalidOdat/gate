#include <string>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

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
    Application::getWindow().setTitle("Hi");

    Logger::info("EditorLayer::onAttach was called");

    mBoard.push_component(new Component{"Hello"});
  }

  void EditorLayer::onDetach() {
  }

  void EditorLayer::onUpdate(Timestep ts) {
    Application::getRenderer().begin(mEditorCameraController.getCamera());

    Application::getRenderer().clearScreen();
    mBoard.renderAll(Application::getRenderer());
    Application::getRenderer().drawQuad(mLastPosition - Vec2{5, 5}/2.0f, mSize, Color::BLACK);

    auto width  = Application::getWindow().getWidth();
    auto height = Application::getWindow().getHeight();

    // Selector
    Application::getRenderer().drawCenteredQuad(getGridAlignedMousePosition(), {8, 8}, Color::BLUE);

    // TODO: Move to UI
    if (mClicked) {
      const StringView text = "Press <ESCAPE> to cancel";
      const auto size = 20;
      const auto x = size * (text.size()/2);
      Application::getRenderer().drawText(text, {x, height - size*2}, 20, Color::BLACK);
    }
    std::stringstream ss;
    ss.precision(2);
    ss << "Title" << " - " << std::fixed << (1.0f / Timestep::get()) << "fps / " << Timestep::get() * 1000.0f << "ms";
    std::string fpsString = ss.str();
    Application::getWindow().setTitle(fpsString.c_str());
  }

  void EditorLayer::onUiRender(Ui& ui) {
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
    mBoard.invalidate(width, height);
    return false;
  }

  bool EditorLayer::onMouseScrollEvent(const MouseScrollEvent& event) {
    (void)event;
    return false;
  }

  bool EditorLayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    if (event.getKey() == Key::Escape) {
      Application::get().quit();
    }

    if (event.getKey() == Key::F3) {
      mShow = !mShow;
      return true;
    }
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

      mLastPosition = mLastMousePosition;
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
    Vec2 position = gridAlginPosition(event.toVec2());
    if (mClicked) {
      Vec2 temp = mLastPosition - position;
      if (temp.x < temp.y) {
        mSize = {position.x - mLastPosition.x, 5};
      } else {
        mSize = {5, position.y - mLastPosition.y};
      }
    }
    mLastMousePosition = position;
    return false;
  }

  Vec2 EditorLayer::gridAlginPosition(Vec2 position) {
    return Vec2{
      u32(position.x) - u32(position.x) % Board::GRID_SIZE,
      u32(position.y) - u32(position.y) % Board::GRID_SIZE
    };
  }
  Vec2 EditorLayer::getGridAlignedMousePosition() {
    return gridAlginPosition(mLastMousePosition);
  }

} // namespace Gate
