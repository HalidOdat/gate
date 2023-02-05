#include "Core/Base.hpp"
#include "Events/WindowEvent.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Material.hpp"
#include "Application.hpp"

#include "Core/OpenGL.hpp"

#include <cstdlib>
#include <sstream>
#include <iomanip>

namespace Gate {

  Application* Application::sInstance = nullptr;

  void Application::start() {
    this->layer = new EditorLayer();

    startGameLoop();
  }

  Application::Application(StringView title, u32 width, u32 height) {
    Logger::trace("Game Engine Initializing...");

    sInstance = this;
    this->mTitle = title;
    this->window = Window::create(title.data(), width, height);
    if (!this->window) {
      std::exit(EXIT_FAILURE);
    }

    #ifdef GATE_PLATFORM_WEB
      Shader::globalDefine("WEB_GL", "1");
    #endif

    this->mRenderer2D = new Renderer2D();
    this->ui = new Ui(width, height);

    this->window->setEventCallback(
      [this](const Event& event) { this->onEvent(event); }
    );

    Material::initialize();

    Logger::info("Game Engine Initialized!");
  }

  Application::~Application() {
    Logger::trace("Game Engine Terminating...");

    delete this->layer;
    delete this->mRenderer2D;
    if (this->mRenderer3D) {
      delete mRenderer3D;
    }
    delete this->ui;

    Logger::trace("Destroying all materials");
    Material::destroyAll();
    Logger::trace("Destroying all textures");
    Texture::destroyAllTextures();
    Logger::trace("Destroying all meshes");
    Mesh::destroyAllMeshes();
    this->window.reset();

    Logger::info("Game Engine Terminated!");
  }

  Renderer3D& Application::getRenderer3D() {
    if (!sInstance->mRenderer3D) {
      sInstance->mRenderer3D = new Renderer3D();
    }
    return *sInstance->mRenderer3D;
  }

  void Application::gameLoop() {
    Application* self = sInstance;

    float time = (float)glfwGetTime();
    float dt = time - self->lastFrameTime;
    self->lastFrameTime = time;
    Timestep::timestep = dt;

    #ifdef GATE_DEBUG_MODE
      std::stringstream ss;
      ss.precision(2);
      ss << self->mTitle << " - " << std::fixed << (1.0f / Timestep::get()) << "fps / " << Timestep::get() * 1000.0f << "ms";
      std::string fpsString = ss.str();
      Application::getWindow().setTitle(fpsString.c_str());
    #endif

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!self->mWindowMinimized) {
      self->layer->onUpdate(dt);

      // self->ui->prepareFrame();
      // self->layer->onUiRender(*self->ui);
      // self->ui->endFrame();

      if (self->mRenderer3D) {
        self->mRenderer3D->waitAndRender();
      }
      self->mRenderer2D->end();
    }
    self->window->update();
  }

  void Application::startGameLoop() {
    this->lastFrameTime = (float)glfwGetTime();
    this->window->setVSync(true);
    this->window->setTitle(mTitle);

    getRenderer2D().blending(true);

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

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  
  Gate::Application("Logic Gate Simulator", 840, 640).start();
}
