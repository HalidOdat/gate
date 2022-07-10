#pragma once

#include "Core/Type.hpp"

#include <memory>

// Forward declare
struct GLFWwindow;

namespace Game {

  class Window {
  public:
    static std::unique_ptr<Window> create(const char* title, const u32 width, const u32 height);
    Window(GLFWwindow* window)
            : window{ window }
    {}
    ~Window();

    bool shouldClose();
    void setShouldClose();
    void update();

  private:
    GLFWwindow* window;
  };

} // namespace Game
