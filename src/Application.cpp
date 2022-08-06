#include <cstdlib>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Events/WindowEvent.hpp"
#include "Resource/Manager.hpp"
#include "Renderer/Renderer.hpp"
#include "Application.hpp"

#include "Layers/GameLayer.hpp"
#include "Layers/EditorLayer.hpp"

namespace Game {

  Application* Application::sInstance = nullptr;

  void Application::start() {
    this->pushLayer(new EditorLayer());

    startGameLoop();
  }

  Application::Application(const char *title, const u32 width, const u32 height) {
    Logger::trace("Game Engine Initializing...");

    sInstance = this;
    this->window = Window::create(title, width, height);
    if (!this->window) {
      std::exit(EXIT_FAILURE);
    }

    ResourceManager::initialize();
    Renderer::initialize();

    this->ui = new Ui(width, height);

    std::filesystem::current_path("C:\\linux\\github\\opengl-game");

    this->window->setEventCallback(
      [this](const Event& event) { this->onEvent(event); }
    );
    
    Logger::info("Game Engine Initialized!");
  }

  Application::~Application() {
    Logger::trace("Game Engine Terminating...");

    this->layerStack.clear();
    delete this->ui;
    Renderer::shutdown();
    ResourceManager::shutdown();
    this->window.reset();

    Logger::info("Game Engine Terminated!");
  }

  void Application::startGameLoop() {
    this->lastFrameTime = (float)glfwGetTime();
    while (running) {
      float time = (float)glfwGetTime();
      float dt = time - this->lastFrameTime;
      this->lastFrameTime = time;
      Timestep::timestep = dt;

      // clear all relevant buffers
      // GAME_GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
      // GAME_GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
      if (!mWindowMinimized) {
        this->layerStack.onUpdate(dt);

        Renderer::waitAndRender();

        this->ui->prepareFrame();
        this->layerStack.onUiRender(*this->ui);
        this->ui->endFrame();
      }

      this->window->update();
    }
  }

  void Application::quit() {
    this->running = false;
  }

  void Application::onEvent(const Event& event) {
    event.dispatch(&Application::onWindowResizeEvent, this);
    event.dispatch(&Application::onWindowCloseEvent, this);
    event.dispatch(&Application::onWindowMinimizedEvent, this);

    this->ui->onEvent(event);
    this->layerStack.onEvent(event);
  }

  bool Application::onWindowCloseEvent(const WindowCloseEvent& event) {
    this->running = false;
    return true;
  }

  bool Application::onWindowResizeEvent(const WindowResizeEvent& event) {
    glViewport(0, 0, event.getWidth(), event.getHeight());
    Renderer::invalidate(event.getWidth(), event.getHeight());
    mWindowMinimized = false;
    return false;
  }

  bool Application::onWindowMinimizedEvent(const WindowMinimizedEvent& event) {
    mWindowMinimized = true;
    return false;
  }

  void Application::pushLayer(Layer* layer) {
    this->layerStack.push(layer);
  }

} // namespace Game