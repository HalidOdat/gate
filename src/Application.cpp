#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Events/WindowEvent.hpp"
#include "Application.hpp"

namespace Game {

  Application::Application(const char *title, const u32 width, const u32 height) {
    Logger::trace("Gear Game Engine Initializing...");
    Logger::info("GLFW Version: %s", glfwGetVersionString());
    if (!glfwInit()) {
      const char* description = nullptr;
      glfwGetError(&description);
      GAME_DEBUG_ASSERT(description != nullptr);
      Logger::error("Could not initialize GLFW: '%s'", description);
      std::exit(EXIT_FAILURE);
    } else {
      Logger::trace("GLFW initialized!");
    }

    this->window = Window::create(title, width, height);
    if (!this->window) {
      std::exit(EXIT_FAILURE);
    }

    this->window->setEventCallback(
      [this](const Event& event) { this->onEvent(event); }
    );

    glEnable(GL_BLEND);

    Logger::info("Gear Game Engine Initialized!");
  }

  Application::~Application() {
    Logger::trace("Gear Game Engine Terminating...");

    this->layerStack.clear();

    glfwTerminate();
    const char* description = nullptr;
    glfwGetError(&description);
    if (description) {
      Logger::warn("Could not terminate GLFW: '%s'", description);
    }

    Logger::info("Gear Game Engine Terminated!");
  }

  void Application::start() {
    while (running) {
      GAME_GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
      GAME_GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

      this->layerStack.onUpdate();

      this->window->update();
    }
  }

  void Application::quit() {
    this->window->setShouldClose();
  }

  void Application::onEvent(const Event& event) {
    event.dispatch<WindowResizeEvent>(&Application::onWindowResizeEvent, this);
    event.dispatch<WindowCloseEvent>(&Application::onWindowCloseEvent, this);
    
    Logger::info("Event: %s", event.getName());

    this->layerStack.onEvent(event);
  }

  bool Application::onWindowCloseEvent(const WindowCloseEvent& event) {
    this->running = false;
    return true;
  }

  bool Application::onWindowResizeEvent(const WindowResizeEvent& event) {
    glViewport(0, 0, event.getWidth(), event.getHeight());
    return false;
  }


  void Application::pushLayer(Layer* layer) {
    this->layerStack.push(layer);
  }

} // namespace Game