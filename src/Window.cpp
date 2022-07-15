#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Window.hpp"
#include "Core/Log.hpp"
#include "Core/Assert.hpp"

namespace Game {
  std::unique_ptr<Window> Window::create(const char* title, u32 width, u32 height) {
    Logger::trace("Creating window...");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    #ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    auto window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
      const char* description;
      glfwGetError(&description);
      Logger::error("could not create GLFW window: '%s'", description);
      return nullptr;
    }

    glfwMakeContextCurrent(window);
    GAME_ASSERT(glfwGetError(NULL) == 0);

    if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
    {
      Logger::error("Glad failed");
      return nullptr;
    }

    Logger::info("OpenGL Version: %s", glGetString(GL_VERSION));

    Logger::info("Created window!");
    return std::make_unique<Window>(window);
  }

  bool Window::shouldClose() {
    return (bool)glfwWindowShouldClose(this->window);
  }

  void Window::update() {
    glfwPollEvents();
    glfwSwapBuffers(this->window);
  }

  Window::~Window() {
    glfwDestroyWindow(this->window);
  }

  void Window::setShouldClose() {
    glfwSetWindowShouldClose(this->window, true);
  }
}