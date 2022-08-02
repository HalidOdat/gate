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

    std::stringstream ss2;
    ss2.precision(4);
    ss2 << std::fixed << mValue;

    const auto ar = Application::getWindow().getAspectRatio();
    Renderer::begin(mCameraController.getCamera());
    Renderer::drawText(fpsString, { 0.0f, 0.7f, 0.0f }, { 0.08f, 0.08f});
    Renderer::drawText(ss2.str(), { 0.0f, 0.5f, 0.0f }, { 0.08f, 0.08f});
    // Renderer::drawQuad({0, 0}, {0.9, 0.9});
    Renderer::end();
  }

  void EditorLayer::onUiRender(Ui& ui) {
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

      if (ui.slider(mValue, -100, 100)) {
        Logger::info("Changed!!!");
      }
      // ui.beginLayout(Ui::Layout::Type::Vertical);
      //   if (ui.button({1.0f, 1.0f, 0.0f}, 0)) {
      //     Logger::info("Button 0 clicked!!!");
      //   }
      //   if (ui.button({1.0f, 0.0f, 0.0f}, 1)) {
      //     Logger::info("Button 1 clicked!!!");
      //   }
      //   if (ui.button({1.0f, 0.0f, 1.0f}, 2)) {
      //     Logger::info("Button 2 clicked!!!");
      //   }
      //   if (ui.button({0.0f, 0.0f, 1.0f}, 3)) {
      //     Logger::info("Button 3 clicked!!!");
      //   }
      // ui.endLayout();
      // ui.beginLayout(Ui::Layout::Type::Vertical);
      //   if (ui.button({0.0f, 0.0f, 0.0f}, 4)) {
      //     Logger::info("Button 4 clicked!!!");
      //   }
      //   if (ui.button({1.0f, 1.0f, 1.0f}, 5)) {
      //     Logger::info("Button 5 clicked!!!");
      //   }
      //   if (ui.button({0.5f, 0.5f, 0.5f}, 6)) {
      //     Logger::info("Button 6 clicked!!!");
      //   }
      //   if (ui.button({0.0f, 0.5f, 0.0f}, 7)) {
      //     Logger::info("Button 7 clicked!!!");
      //   }
      //   ui.beginLayout(Ui::Layout::Type::Horizontal);
      //     if (ui.button({1.0f, 1.0f, 0.5f}, 8)) {
      //       Logger::info("Button 8 clicked!!!");
      //     }
      //     if (ui.button({1.0f, 0.5f, 0.5f}, 9)) {
      //       Logger::info("Button 9 clicked!!!");
      //     }
      //   ui.endLayout();
      // ui.endLayout();
    ui.end();
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
    if (event.getKey() == Key::R) {
      ResourceManager::reloadTextures();
      return true;
    }

    return false;
  }

} // namespace Game