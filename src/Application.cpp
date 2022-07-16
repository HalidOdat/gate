#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
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

    glEnable(GL_BLEND);

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
      GAME_GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
      GAME_GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

      this->onUpdate();

      this->window->update();
    }

    this->onDestroy();
  }

  void Application::quit() {
    this->window->setShouldClose();
  }

} // namespace Game