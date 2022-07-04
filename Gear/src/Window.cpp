#include <Gear/Window.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Gear {
  Window::Window(const char* title, u32 width, u32 height) {
    this->window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!this->window) {
      /// Handle error
      throw 404;
    }
  }

  bool Window::shouldClose() {
      return (bool)glfwWindowShouldClose(this->window);
  }

  void Window::pollEvents() {
    glfwPollEvents();
  }
}