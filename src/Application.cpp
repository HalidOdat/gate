#include "Core/Base.hpp"
#include "Events/WindowEvent.hpp"
#include "Resource/Manager.hpp"
#include "Renderer/Renderer.hpp"
#include "Application.hpp"

#include "EditorLayer.hpp"

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
    this->layer = new EditorLayer();
    this->layer->onAttach();

    startGameLoop();
  }

  Application::Application(const char *title, const u32 width, const u32 height) {
    Logger::trace("Game Engine Initializing...");

    sInstance = this;
    this->window = Window::create(title, width, height);
    if (!this->window) {
      std::exit(EXIT_FAILURE);
    }

    #ifdef GAME_PLATFORM_WEB
      Shader::globalDefine("WEB_GL", "1");
    #endif

    ResourceManager::initialize();
    Renderer::initialize();

    this->ui = new Ui(width, height);

    this->window->setEventCallback(
      [this](const Event& event) { this->onEvent(event); }
    );

    Logger::info("Game Engine Initialized!");
  }

  Application::~Application() {
    Logger::trace("Game Engine Terminating...");

    // TOOD: use unique_ptr here
    this->layer->onDetach();
    delete this->layer;

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
      self->layer->onUpdate(dt);

      Renderer::waitAndRender();

      self->ui->prepareFrame();
      self->layer->onUiRender(*self->ui);
      self->ui->endFrame();
    }
    self->window->update();
  }

  void Application::startGameLoop() {
    this->lastFrameTime = (float)glfwGetTime();

    #ifndef GAME_PLATFORM_WEB
      // this->window->setVSync(true);
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
    this->running = false;
  }

  void Application::onEvent(const Event& event) {
    event.dispatch(&Application::onWindowResizeEvent, this);
    event.dispatch(&Application::onWindowCloseEvent, this);
    event.dispatch(&Application::onWindowMinimizedEvent, this);

    this->ui->onEvent(event);
    this->layer->onEvent(event);
  }

  bool Application::onWindowCloseEvent(const WindowCloseEvent&) {
    this->running = false;
    return true;
  }

  bool Application::onWindowResizeEvent(const WindowResizeEvent& event) {
    glViewport(0, 0, event.getWidth(), event.getHeight());
    Renderer::invalidate(event.getWidth(), event.getHeight());
    mWindowMinimized = false;
    return false;
  }

  bool Application::onWindowMinimizedEvent(const WindowMinimizedEvent&) {
    mWindowMinimized = true;
    return false;
  }

} // namespace Game

