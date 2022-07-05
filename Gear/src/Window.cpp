#include <Gear/Window.hpp>
#include <Gear/Core/Log.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Gear {
  std::unique_ptr<Window> Window::create(const char* title, u32 width, u32 height) {
    Logger::trace("Creating window...");
    auto window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
      const char* description;
      glfwGetError(&description);
      Logger::error("could not create GLFW window: '%s'", description);
      return nullptr;
    }
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