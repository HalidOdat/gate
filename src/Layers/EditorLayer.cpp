#include <string>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Renderer/Renderer.hpp"
#include "Resource/Manager.hpp"
#include "Layers/EditorLayer.hpp"

#include "Application.hpp"

namespace Game {

  EditorLayer::EditorLayer()
    : mCameraController(Application::getWindow().getAspectRatio())
  {}

  void EditorLayer::onAttach() {
    Logger::info("EditorLayer::onAttach was called");
  }

  void EditorLayer::onDetach() {
    Logger::info("EditorLayer::onDetach was called");
  }

  void EditorLayer::onUpdate(Timestep ts) {
    auto fps = 1.0f / ts;
    std::stringstream ss;
    ss.precision(2);
    ss << std::fixed << (1.0f / ts) << "fps" << '\n';

    std::string fpsString = ss.str();

    const auto ar = Application::getWindow().getAspectRatio();
    Renderer::begin(mCameraController.getCamera());
    Renderer::drawText(fpsString, { 0.0f, 0.7f, 0.0f }, { 0.08f, 0.08f});
  }

  void EditorLayer::onUiRender(Ui& ui) {
    if (mShow) {
      ui.begin({0.0f, 0.0f});
        if (ui.button("Button 0", 0)) {
          Logger::info("Button 0 clicked!!!");
        }
        if (ui.button("Button 1", 1)) {
          Logger::info("Button 1 clicked!!!");
        }
        if (ui.button("Button 2", 2)) {
          Logger::info("Button 2 clicked!!!");
        }
        if (ui.button("Button 3", 3)) {
          Logger::info("Button 3 clicked!!!");
        }
      ui.end();
    }
  }

  void EditorLayer::onEvent(const Event& event) {
    event.dispatch(&EditorLayer::onWindowResizeEvent, this);
    event.dispatch(&EditorLayer::onMouseScrollEvent, this);
    event.dispatch(&EditorLayer::onKeyPressedEvent, this);
  }

  bool EditorLayer::onWindowResizeEvent(const WindowResizeEvent& event) {
    auto[width, height] = event.getSize();
    mCameraController.resize(width, height);
    return false;
  }

  bool EditorLayer::onMouseScrollEvent(const MouseScrollEvent& event) {
    return false;
  }

  bool EditorLayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    if (event.getKey() == Key::F3) {
      mShow = !mShow;
      return true;
    }

    if (mShow) {
      if (event.getKey() == Key::T) {
        ResourceManager::reloadAll<Texture2D>();
        return true;
      }
      if (event.getKey() == Key::Z) {
        ResourceManager::reloadAll<Shader>();
        return true;
      }
      if (event.getKey() == Key::M) {
        ResourceManager::reloadAll<Mesh>();
        return true;
      }
    }

    return false;
  }

} // namespace Game