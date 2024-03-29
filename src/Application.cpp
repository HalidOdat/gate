#include "Core/Base.hpp"
#include "Events/WindowEvent.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Material.hpp"
#include "Application.hpp"
#include <cstdlib>
#include <sstream>
#include <iomanip>

#include "Core/OpenGL.hpp"

#ifdef GATE_PLATFORM_WEB
#   include <emscripten/emscripten.h>
#   if defined(__cplusplus)
#   	  define EM_PORT_API(rettype) extern "C" rettype EMSCRIPTEN_KEEPALIVE
#   else
#   	  define EM_PORT_API(rettype) rettype EMSCRIPTEN_KEEPALIVE
#   endif
#endif

static Gate::u32 canvasWidth  = 840; 
static Gate::u32 canvasHeight = 640;

#ifdef GATE_PLATFORM_WEB
  // The reason for string property access: https://github.com/emscripten-core/emscripten/issues/16390
  EM_JS(int, canvas_get_width, (), {
    return Module['canvas'].width;
  });

  EM_JS(int, canvas_get_height, (), {
    return Module['canvas'].height;
  });

  EM_JS(void, Module_saveFile, (const char *content), {
    Module['saveFile'](UTF8ToString(content));
  });

  EM_JS(void, Module_renameChip, (), {
    Module['renameChip']();
  });

  EM_PORT_API(void) gate_resizeWindow(int width, int height) {
    Gate::Application::getWindow().setSize(width, height);
  }

  EM_PORT_API(void) gate_renameChip(char* name) {
    Gate::Application::renameChip(name);
  }
#endif

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

    Logger::trace("Destroying all materials");
    Material::destroyAll();
    Logger::trace("Destroying all textures");
    Texture::destroyAllTextures();
    Logger::trace("Destroying all meshes");
    Mesh::destroyAllMeshes();
    this->window.reset();

    Logger::info("Game Engine Terminated!");
  }

  void Application::saveFile(const String& name, const String& content) {
    #ifdef GATE_PLATFORM_WEB
      (void) name;
      Module_saveFile(content.c_str());
    #else
      FILE* file = fopen(name.c_str(), "w+");
      if (fwrite(content.c_str(), sizeof(char), content.size(), file) != sizeof(char) * content.size()) {
        Logger::error("Unable to write to json file");
      }
      fclose(file);
    #endif
  }

  void Application::renameChip() {
    #ifdef GATE_PLATFORM_WEB
      Module_renameChip();
    #else
      GATE_TODO("not implemented");
    #endif
  }

  void Application::renameChip(const String& name) {
    Application::get().layer->getBoard().getCurrentChip().setName(name);
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

    this->layer->onEvent(event);
  }

  bool Application::onWindowCloseEvent(const WindowCloseEvent&) {
    this->running = false;
    return true;
  }

  bool Application::onWindowResizeEvent(const WindowResizeEvent& event) {
    glViewport(0, 0, event.getWidth(), event.getHeight());
    mWindowMinimized = false;
    mRenderer2D->invalidate(event.getWidth(), event.getHeight());
    if (mRenderer3D) {
      mRenderer3D->invalidate(event.getWidth(), event.getHeight());
    }
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
  
#ifdef GATE_PLATFORM_WEB
  canvasWidth  = (Gate::u32)canvas_get_width();
  canvasHeight = (Gate::u32)canvas_get_height();
#endif

  Gate::Application("Logic Gate Simulator", canvasWidth, canvasHeight).start();
}
