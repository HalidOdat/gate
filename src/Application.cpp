#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Events/WindowEvent.hpp"
#include "Renderer/Renderer.hpp"
#include "Application.hpp"

namespace Game {

  Application::Application(const char *title, const u32 width, const u32 height) {
    Logger::trace("Game Engine Initializing...");

    this->window = Window::create(title, width, height);
    if (!this->window) {
      std::exit(EXIT_FAILURE);
    }

    this->window->setEventCallback(
      [this](const Event& event) { this->onEvent(event); }
    );

    Renderer::Initialize();

    this->ui = new Ui(-1.0f, 1.0f, -1.0f, 1.0f);

    Logger::info("Game Engine Initialized!");
  }

  Application::~Application() {
    Logger::trace("Game Engine Terminating...");

    this->layerStack.clear();
    delete this->ui;
    Renderer::Shutdown();
    this->window.reset();

    Logger::info("Game Engine Terminated!");
  }

  void Application::start() {
    while (running) {
      GAME_GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
      GAME_GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

      this->layerStack.onUpdate();
      this->layerStack.onUiRender(*this->ui);

      this->window->update();
    }
  }

  void Application::quit() {
    this->window->setShouldClose();
  }

  void Application::onEvent(const Event& event) {
    event.dispatch<WindowResizeEvent>(&Application::onWindowResizeEvent, this);
    event.dispatch<WindowCloseEvent>(&Application::onWindowCloseEvent, this);

    this->ui->onEvent(event);
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