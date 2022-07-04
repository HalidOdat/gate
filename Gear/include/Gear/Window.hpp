#pragma once

#include <Gear/Core/Type.hpp>

// Forward declare
struct GLFWwindow;

namespace Gear {

  class Window {
  public:
    Window(const char* title, u32 width, u32 height);
    ~Window();

    bool shouldClose();
    void setShouldClose();

    void update();

  private:
    GLFWwindow* window;
  };

} // namespace Gear
