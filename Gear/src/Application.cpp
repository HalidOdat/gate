#include <Gear/Application.hpp>
#include <Gear/Core/Log.hpp>

#include <GLFW/glfw3.h>

#include <cstdlib>

namespace Gear {

  Application::Application(const char *title, const u32 width, const u32 height) {
    Logger::trace("Gear Game Engine Initializing...");
    if (!glfwInit()) {
      const char* description;
      glfwGetError(&description);
      Logger::error("Could not initialize GLFW: '%s'", description);
      std::exit(EXIT_FAILURE);
    }

    this->window = Window::create(title, width, height);
    if (!this->window) {
      std::exit(EXIT_FAILURE);
    }

    Logger::info("Gear Game Engine Initialized!");
  }

  Application::~Application() {
    Logger::trace("Gear Game Engine Terminating...");

    glfwTerminate();
    const char* description = nullptr;
    glfwGetError(&description);
    if (description) {
      Logger::warn("Could not terminate GLFW: '%s'", description);
    }

    Logger::info("Gear Game Engine Terminated!");
  }

  void Application::start() {
    this->onCreate();

    while (!this->window->shouldClose()) {
      this->onUpdate();

      this->window->update();
    }

    this->onDestroy();
  }

  void Application::quit() {
    this->window->setShouldClose();
  }

} // namespace Gear