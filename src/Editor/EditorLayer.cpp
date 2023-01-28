#include <string>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Core/Base.hpp"
#include "Core/Input.hpp"
#include "Resource/Manager.hpp"
#include "Application.hpp"
#include "Editor/EditorLayer.hpp"

namespace Game {

  EditorLayer::EditorLayer()
    : mEditorCameraController(Application::getWindow().getAspectRatio())
    , mCameraController(Vec3{0.0f, 0.0f, 3.0f}, 45.0f, Application::getWindow().getAspectRatio())
  {}

  void EditorLayer::onAttach() {
    Application::getWindow().setTitle("Hi");

    Logger::info("EditorLayer::onAttach was called");
  }

  void EditorLayer::onDetach() {
  }

  void EditorLayer::onUpdate(Timestep ts) {
    if (!Input::isKeyPressed(Key::LeftControl) && !Input::isKeyPressed(Key::RightControl)) {
      mCameraController.onUpdate(ts);
    }
    glEnable(GL_BLEND);
    Application::getRenderer().begin(mEditorCameraController.getCamera());

    Application::getRenderer().clearScreen();
    Application::getRenderer().drawText("This is a square", {200, 200}, 20, Color::BLACK);
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
    if (mClicked) {
      mCameraController.onEvent(event);
    } else {
      mCameraController.resetLastPosition();
    }

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
    mLastMousePosition = {event.getX(), event.getY()};
    return false;
  }

} // namespace Game
