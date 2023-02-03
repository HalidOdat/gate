#include "Core/Base.hpp"
#include "Events/WindowEvent.hpp"
#include "Renderer/Texture.hpp"
#include "Application.hpp"

#include "Core/OpenGL.hpp"

#include <cstdlib>

namespace Gate {

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

    #ifdef GATE_PLATFORM_WEB
      Shader::globalDefine("WEB_GL", "1");
    #endif

    this->mRenderer = new Renderer();
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
    delete this->mRenderer;
    delete this->ui;
    Logger::trace("Destroying all textures");
    Texture::destroyAllTextures();
    this->window.reset();

    Logger::info("Game Engine Terminated!");
  }

  void Application::gameLoop() {
    Application* self = sInstance;
    #ifdef GATE_PLATFORM_WEB
      self->window->setVSync(true);
    #endif

    float time = (float)glfwGetTime();
    float dt = time - self->lastFrameTime;
    self->lastFrameTime = time;
    Timestep::timestep = dt;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!self->mWindowMinimized) {
      self->layer->onUpdate(dt);

      // self->ui->prepareFrame();
      // self->layer->onUiRender(*self->ui);
      // self->ui->endFrame();

      self->mRenderer->end();
    }
    self->window->update();
  }

  void Application::startGameLoop() {
    this->lastFrameTime = (float)glfwGetTime();

    #ifndef GATE_PLATFORM_WEB
      // this->window->setVSync(true);
    #endif

    getRenderer().blending(true);

    #if GATE_PLATFORM_WEB
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
    mWindowMinimized = false;
    return false;
  }

  bool Application::onWindowMinimizedEvent(const WindowMinimizedEvent&) {
    mWindowMinimized = true;
    return false;
  }

} // namespace Gate

