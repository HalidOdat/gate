#include "Core/Base.hpp"
#include "Events/WindowEvent.hpp"
#include "Resource/Manager.hpp"
#include "Renderer/Renderer.hpp"
#include "Application.hpp"

#include "Layers/GameLayer.hpp"
#include "Layers/EditorLayer.hpp"

#if GAME_PLATFORM_WEB
# include<emscripten/emscripten.h>
// # define GLFW_INCLUDE_ES3
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <filesystem>

namespace Game {

  Application* Application::sInstance = nullptr;

  void Application::start() {
    GAME_PROFILE_FUNCTION();
    
    this->pushLayer(new EditorLayer());

    startGameLoop();
  }

  Application::Application(const char *title, const u32 width, const u32 height) {
    GAME_PROFILE_FUNCTION();

    Logger::trace("Game Engine Initializing...");

    sInstance = this;
    this->window = Window::create(title, width, height);
    if (!this->window) {
      std::exit(EXIT_FAILURE);
    }

    ResourceManager::initialize();
    Renderer::initialize();

    this->ui = new Ui(width, height);

    this->window->setEventCallback(
      [this](const Event& event) { this->onEvent(event); }
    );
    
    Logger::info("Game Engine Initialized!");
  }

  Application::~Application() {
    GAME_PROFILE_FUNCTION();

    Logger::trace("Game Engine Terminating...");

    this->layerStack.clear();
    delete this->ui;
    Renderer::shutdown();
    ResourceManager::shutdown();
    this->window.reset();

    Logger::info("Game Engine Terminated!");
  }

  void Application::gameLoop() {
    Application* self = sInstance;
    #ifdef GAME_PLATFORM_WEB
      self->window->setVSync(true);
    #endif

    float time = (float)glfwGetTime();
    float dt = time - self->lastFrameTime;
    self->lastFrameTime = time;
    Timestep::timestep = dt;

    if (!self->mWindowMinimized) {
      self->layerStack.onUpdate(dt);

      Renderer::waitAndRender();

      self->ui->prepareFrame();
      self->layerStack.onUiRender(*self->ui);
      self->ui->endFrame();
    }

    self->window->update();
  }

  void Application::startGameLoop() {
    GAME_PROFILE_FUNCTION();

    this->lastFrameTime = (float)glfwGetTime();

    #ifndef GAME_PLATFORM_WEB
      this->window->setVSync(true);
    #endif

  #if GAME_PLATFORM_WEB
    emscripten_set_main_loop(Application::gameLoop, 0, 1);
#else
    while (running) {
      Application::gameLoop();
    }
#endif
  }

  void Application::quit() {
    GAME_PROFILE_FUNCTION();
    this->running = false;
  }

  void Application::onEvent(const Event& event) {
    GAME_PROFILE_FUNCTION();

    event.dispatch(&Application::onWindowResizeEvent, this);
    event.dispatch(&Application::onWindowCloseEvent, this);
    event.dispatch(&Application::onWindowMinimizedEvent, this);

    this->ui->onEvent(event);
    this->layerStack.onEvent(event);
  }

  bool Application::onWindowCloseEvent(const WindowCloseEvent&) {
    GAME_PROFILE_FUNCTION();
    this->running = false;
    return true;
  }

  bool Application::onWindowResizeEvent(const WindowResizeEvent& event) {
    GAME_PROFILE_FUNCTION();
    glViewport(0, 0, event.getWidth(), event.getHeight());
    Renderer::invalidate(event.getWidth(), event.getHeight());
    mWindowMinimized = false;
    return false;
  }

  bool Application::onWindowMinimizedEvent(const WindowMinimizedEvent&) {
    GAME_PROFILE_FUNCTION();
    mWindowMinimized = true;
    return false;
  }

  void Application::pushLayer(Layer* layer) {
    GAME_PROFILE_FUNCTION();
    this->layerStack.push(layer);
  }

} // namespace Game