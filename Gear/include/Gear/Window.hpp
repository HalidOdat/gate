#pragma once

#include <Gear/Core.hpp>

struct GLFWwindow;

namespace Gear {

  class Window {
  public:
    Window(const char* title, u32 width, u32 height);

    bool shouldClose();
    void pollEvents();

  private:
    GLFWwindow* window;
  };

} // namespace Gear
